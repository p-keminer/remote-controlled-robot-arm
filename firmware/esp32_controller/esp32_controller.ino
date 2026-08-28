#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_mac.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Preferences.h>
#include <Adafruit_NeoPixel.h>

// Controller: IMU-Daten lesen und per ESP-NOW senden
// I2C: SDA GPIO8 / SCL GPIO9 | Mux: 0x70 | Sensoren: 0x29
// Peer-Adresse kommt aus peer_config.local.h (gitignored)
// Vorlage: peer_config.template.h
//
// Kalibrierung: Offsets werden im NVS gespeichert und beim Boot geladen.
// Befehle ueber Serial:
//   RECAL    — alle Offsets loeschen, Neukalibrierung
//   CAL0     — Einzelkalibrierung Sensor 0 (Mux bleibt offen)
//   CAL1     — Einzelkalibrierung Sensor 1
//   CAL2     — Einzelkalibrierung Sensor 2
//   STOP     — Einzelkalibrierung abbrechen, Normalbetrieb
//
// Aktuelle Segmentzuordnung (Stand 2026-04-22):
//   S0 = Hand/Wrist (Mux-Kanal 0)
//   S1 = Unterarm   (Mux-Kanal 1)
//   S2 = Oberarm    (Mux-Kanal 2)
//
// LED-Debugging (alle mit 100 Ohm Vorwiderstand):
//   GPIO4  Blau   — Oberarm-IMU (S2)
//   GPIO5  Rot    — Unterarm-IMU (S1)
//   GPIO6  Weiss  — Hand/Wrist-IMU (S0)
//   GPIO7  Gruen  — COMMS
//   GPIO10 ?      — FAULT (externe Farbe separat pruefen)
//
// Notaus-Schalter (GPIO21):
//   Toggle-Button nach GND mit internem Pull-Up.
//   Jeder Tastendruck toggelt den Notaus-Zustand.
//   Entprellung: 50ms, Zustandswechsel wird geloggt und per FAULT-LED angezeigt.
//   LED-Schema: AUS = OK, AN = Problem/Notaus.

#include "peer_config.local.h"

#define I2C_SDA           8
#define I2C_SCL           9
#define MUX_ADRESSE       0x70
#define BNO_ADRESSE       0x29
#define ANZAHL_SENSOREN   3
#define SENDE_INTERVALL   5     // ms
#define PROTOKOLL_VERSION 4

#define NOTAUS_PIN       21     // Toggle-Button nach GND, interner Pull-Up
#define NOTAUS_ENTPRELL   50    // ms Entprellzeit

#define FLEX_PIN          1
// Poti-Arbeitsstand 2026-04-23:
// Der defekte Flex-Sensor wird temporaer durch einen 10k-Poti ersetzt.
// Verdrahtung im aktuellen 2-Draht-Aufbau:
//   bestehende 3.3V-Leitung          -> aeusserer Poti-Pin
//   bestehende ADC-/Teiler-Leitung   -> mittlerer Poti-Pin (Wischer)
//   10k Pull-Down gegen GND bleibt auf dem Controller bestehen
//
// Im aktuellen Live-Aufbau wurde gemessen:
//   offen / Ruhelage  ~= 1935 ADC
//   geschlossen / zu  ~= 3020 ADC
// OFFEN/Ruhelage soll 0% liefern, ZU soll Richtung 100% gehen.
#define FLEX_GERADE       1935
#define FLEX_GEBOGEN      3020
#define FLEX_FILTER_ALPHA  0.18f
#define FLEX_RAW_DEADBAND   2.0f

#define KALIB_SCHWELLE_GYRO  3
#define KALIB_SCHWELLE_ACCEL 3
#define KALIB_SCHWELLE_MAG   3
#define KALIB_SAVE_STABLE_COUNT 3

// LED-Pins: historische Namen beibehalten, reale Farben/Zuordnung siehe Header oben
#define LED_HAND          4   // GPIO4  — reale externe LED: Blau   — S2 Oberarm
#define LED_UNTERARM      5   // GPIO5  — reale externe LED: Rot    — S1 Unterarm
#define LED_OBERARM       6   // GPIO6  — reale externe LED: Weiss  — S0 Hand/Wrist
#define LED_COMMS         7   // GPIO7  — reale externe LED: Gruen  — Kommunikation
#define LED_FAULT        10   // GPIO10 — externe FAULT-LED, Farbe separat pruefen
#define RGB_PIN          48   // Interne RGB-LED
#define RGB_ANZAHL        1

// Flags-Bitfeld (ImuPaket v4)
#define FLAG_NOTAUS       (1 << 0)   // Bit 0: Notaus aktiv
// Bits 1-7: reserviert fuer spaetere Erweiterung (Sensor-Gueltigkeit, Auth, etc.)

// Zuordnung: LED-Pin pro Sensor-Index (S0=Hand/Wrist, S1=Unterarm, S2=Oberarm)
Adafruit_NeoPixel rgb(RGB_ANZAHL, RGB_PIN, NEO_GRB + NEO_KHZ800);

static const uint8_t led_sensor_pin[ANZAHL_SENSOREN] = {
    LED_OBERARM,    // S0
    LED_UNTERARM,   // S1
    LED_HAND        // S2
};

static const char* sensor_segment_name[ANZAHL_SENSOREN] = {
    "Hand/Wrist",
    "Unterarm",
    "Oberarm"
};

static const char* sensor_led_name[ANZAHL_SENSOREN] = {
    "Weiss/GPIO6",
    "Rot/GPIO5",
    "Blau/GPIO4"
};

typedef struct {
    float heading;
    float roll;
    float pitch;
} SensorDaten;

typedef struct {
    uint8_t sys;
    uint8_t gyro;
    uint8_t accel;
    uint8_t mag;
} KalibStatus;

typedef struct __attribute__((packed)) {
    uint32_t    zaehler;
    SensorDaten sensoren[ANZAHL_SENSOREN];
    KalibStatus kalib[ANZAHL_SENSOREN];
    float       flex_prozent;
    uint8_t     flags;               // Bitfeld: Bit 0 = Notaus, Bits 1-7 reserviert
    uint8_t     protokoll_version;   // 4
    uint8_t     pruefsumme;
} ImuPaket;

Adafruit_BNO055 sensoren[ANZAHL_SENSOREN] = {
    Adafruit_BNO055(55, BNO_ADRESSE, &Wire),
    Adafruit_BNO055(56, BNO_ADRESSE, &Wire),
    Adafruit_BNO055(57, BNO_ADRESSE, &Wire),
};

Preferences prefs;
static uint32_t sende_zaehler = 0;
static bool kalib_gespeichert[ANZAHL_SENSOREN] = {false, false, false};
static uint8_t kalib_stabil_zaehler[ANZAHL_SENSOREN] = {0, 0, 0};
static bool sensor_bereit[ANZAHL_SENSOREN] = {false, false, false};
static KalibStatus letzter_kalib_status[ANZAHL_SENSOREN] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
static SensorDaten letzter_gueltiger_sensor[ANZAHL_SENSOREN] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};
static bool letzter_gueltiger_sensor_vorhanden[ANZAHL_SENSOREN] = {false, false, false};
static uint32_t sensor_nullframe_hold_zaehler[ANZAHL_SENSOREN] = {0, 0, 0};
static bool letzter_tx_ok = false;
static bool fehler_aktiv = false;
static bool flex_bereit = true;
static bool flex_filter_initialisiert = false;
static float flex_roh_gefiltert = 0.0f;
#if BRIDGE_AKTIV
static bool bridge_registriert = false;
#endif

// Notaus-Zustand (Toggle-Button, entprellt)
static bool notaus_aktiv = false;
static bool notaus_letzter_pegel = HIGH;       // Pull-Up: Ruhezustand = HIGH
static bool notaus_pegel_stabil = HIGH;        // letzter stabiler Pegel
static unsigned long notaus_letzte_aenderung_ms = 0;

// Status-Debug ist im Livebetrieb standardmaessig aus, weil grosse
// Serial-Bloecke messbare Paketpausen verursachen koennen.
#define DEBUG_INTERVALL 2000  // ms
static bool debug_status_aktiv = false;
static unsigned long letzter_debug_ms = 0;
static unsigned long letzter_loop_start_us = 0;
static uint32_t timing_samples = 0;
static uint64_t timing_interval_sum_us = 0;
static uint32_t timing_interval_max_us = 0;
static uint64_t timing_hotpath_sum_us = 0;
static uint32_t timing_hotpath_max_us = 0;

// Langsame Diagnose getrennt vom Bewegungs-Hotpath:
// Pro Intervall wird nur ein Sensor geprueft. Bei drei Sensoren bekommt jeder
// Sensor etwa alle 6s einen frischen Status. Im Livebetrieb ist das aus,
// weil BNO055-Statusabfragen die naechste Paket-Runde sichtbar blockieren.
#define STATUS_CHECK_INTERVALL 2000  // ms
#define MUX_SETTLE_DELAY         5   // ms nach Kanalwechsel
static bool status_diag_aktiv = false;
static unsigned long letzter_status_check_ms = 0;
static uint8_t status_check_sensor = 0;

// Einzelkalibrierungsmodus: -1 = aus, 0/1/2 = aktiver Sensor
static int8_t kalib_einzeln = -1;

void mux_kanal_oeffnen(uint8_t kanal) {
    Wire.beginTransmission(MUX_ADRESSE);
    Wire.write(1 << kanal);
    Wire.endTransmission();
}

void mux_alle_schliessen() {
    Wire.beginTransmission(MUX_ADRESSE);
    Wire.write(0x00);
    Wire.endTransmission();
}

uint8_t pruefsumme_berechnen(const ImuPaket* paket) {
    const uint8_t* bytes = (const uint8_t*)paket;
    uint8_t summe = 0;
    for (size_t i = 0; i < sizeof(ImuPaket) - 1; i++) {
        summe ^= bytes[i];
    }
    return summe;
}

bool kalib_offsets_vorhanden(uint8_t idx) {
    if (idx >= ANZAHL_SENSOREN) return false;

    char key[8];
    snprintf(key, sizeof(key), "cal%d", idx);

    adafruit_bno055_offsets_t offsets;
    return prefs.getBytes(key, &offsets, sizeof(offsets)) == sizeof(offsets);
}

void kalib_laden(uint8_t idx) {
    char key[8];
    snprintf(key, sizeof(key), "cal%d", idx);

    adafruit_bno055_offsets_t offsets;
    size_t len = prefs.getBytes(key, &offsets, sizeof(offsets));
    if (len == sizeof(offsets)) {
        mux_kanal_oeffnen(idx);
        delay(10);
        sensoren[idx].setSensorOffsets(offsets);
        delay(50);
        kalib_gespeichert[idx] = true;
        // Effektive Kalibrierung fuer den Livepfad: gespeicherte Offsets sind
        // geladen, auch wenn der BNO055-Live-Status direkt nach Boot noch 0 meldet.
        letzter_kalib_status[idx] = {3, 3, 3, 3};
        Serial.printf("[KALIB] S%d (%s, %s): Offsets aus NVS geladen -> effektiv kalibriert\n",
            idx, sensor_segment_name[idx], sensor_led_name[idx]);
    } else {
        kalib_gespeichert[idx] = false;
        letzter_kalib_status[idx] = {0, 0, 0, 0};
        Serial.printf("[KALIB] S%d (%s, %s): Keine gespeicherten Offsets\n",
            idx, sensor_segment_name[idx], sensor_led_name[idx]);
    }
}

void kalib_speichern(uint8_t idx) {
    char key[8];
    snprintf(key, sizeof(key), "cal%d", idx);

    adafruit_bno055_offsets_t offsets;
    mux_kanal_oeffnen(idx);
    delay(10);
    sensoren[idx].getSensorOffsets(offsets);
    size_t geschrieben = prefs.putBytes(key, &offsets, sizeof(offsets));
    if (geschrieben == sizeof(offsets)) {
        kalib_gespeichert[idx] = true;
        kalib_stabil_zaehler[idx] = 0;
        Serial.printf("[KALIB] S%d: Offsets in NVS gespeichert!\n", idx);
    } else {
        Serial.printf("[KALIB] FEHLER: S%d Offsets nicht vollstaendig gespeichert (%u/%u Bytes)\n",
            idx, (unsigned)geschrieben, (unsigned)sizeof(offsets));
    }
}

void kalib_alle_loeschen() {
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        char key[8];
        snprintf(key, sizeof(key), "cal%d", i);
        prefs.remove(key);
        kalib_gespeichert[i] = false;
        kalib_stabil_zaehler[i] = 0;
    }
    Serial.println("[KALIB] Alle Offsets geloescht");
}

bool kalib_gut_genug(const KalibStatus* k) {
    return k->gyro >= KALIB_SCHWELLE_GYRO
        && k->accel >= KALIB_SCHWELLE_ACCEL
        && k->mag >= KALIB_SCHWELLE_MAG;
}

void kalib_stabil_reset(uint8_t idx) {
    if (idx < ANZAHL_SENSOREN) {
        kalib_stabil_zaehler[idx] = 0;
    }
}

bool kalib_stabil_genug(uint8_t idx, const KalibStatus* k) {
    if (idx >= ANZAHL_SENSOREN) return false;

    if (!kalib_gut_genug(k)) {
        kalib_stabil_zaehler[idx] = 0;
        return false;
    }

    if (kalib_stabil_zaehler[idx] < KALIB_SAVE_STABLE_COUNT) {
        kalib_stabil_zaehler[idx]++;
    }
    return kalib_stabil_zaehler[idx] >= KALIB_SAVE_STABLE_COUNT;
}

bool kalib_status_live_lesen(uint8_t idx, KalibStatus* k) {
    if (idx >= ANZAHL_SENSOREN || !sensor_bereit[idx] || k == NULL) return false;

    mux_kanal_oeffnen(idx);
    delay(MUX_SETTLE_DELAY);

    sensoren[idx].getCalibration(&k->sys, &k->gyro, &k->accel, &k->mag);
    return true;
}

bool kalib_status_lesen(uint8_t idx) {
    KalibStatus k;
    if (!kalib_status_live_lesen(idx, &k)) return false;

    letzter_kalib_status[idx] = k;
    return true;
}

void kalib_status_alle_ausgeben() {
    Serial.println("[CALSTAT] NVS = gespeicherte Offsets, Live = BNO055-Momentstatus");
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        KalibStatus live = {0, 0, 0, 0};
        bool live_ok = kalib_status_live_lesen(i, &live);
        Serial.printf("[CALSTAT] S%d %-10s LED=%-10s bereit=%s NVS=%s effektiv[S%d G%d A%d M%d]",
            i,
            sensor_segment_name[i],
            sensor_led_name[i],
            sensor_bereit[i] ? "ja" : "nein",
            kalib_offsets_vorhanden(i) ? "ja" : "nein",
            letzter_kalib_status[i].sys,
            letzter_kalib_status[i].gyro,
            letzter_kalib_status[i].accel,
            letzter_kalib_status[i].mag);
        if (live_ok) {
            Serial.printf(" live[S%d G%d A%d M%d]\n", live.sys, live.gyro, live.accel, live.mag);
        } else {
            Serial.println(" live[nicht lesbar]");
        }
    }
    mux_alle_schliessen();
}

void timing_erfassen(uint32_t interval_us, uint32_t hotpath_us) {
    timing_samples++;

    if (interval_us > 0) {
        timing_interval_sum_us += interval_us;
        if (interval_us > timing_interval_max_us) timing_interval_max_us = interval_us;
    }

    timing_hotpath_sum_us += hotpath_us;
    if (hotpath_us > timing_hotpath_max_us) timing_hotpath_max_us = hotpath_us;
}

void timing_zuruecksetzen() {
    timing_samples = 0;
    timing_interval_sum_us = 0;
    timing_interval_max_us = 0;
    timing_hotpath_sum_us = 0;
    timing_hotpath_max_us = 0;
}

bool sensor_nullframe(const SensorDaten& sensor) {
    return sensor.heading == 0.0f && sensor.roll == 0.0f && sensor.pitch == 0.0f;
}

SensorDaten sensorwert_uebernehmen_oder_halten(uint8_t idx, const SensorDaten& gelesen) {
    if (idx >= ANZAHL_SENSOREN) return gelesen;

    if (sensor_nullframe(gelesen)) {
        if (letzter_gueltiger_sensor_vorhanden[idx]) {
            sensor_nullframe_hold_zaehler[idx]++;
            return letzter_gueltiger_sensor[idx];
        }
        return gelesen;
    }

    letzter_gueltiger_sensor[idx] = gelesen;
    letzter_gueltiger_sensor_vorhanden[idx] = true;
    return gelesen;
}

void timing_ausgeben_und_reset() {
    if (timing_samples == 0) {
        Serial.println("[TIMING] Noch keine Samples gesammelt.");
        return;
    }

    float interval_avg_ms = (timing_interval_sum_us / (float)timing_samples) / 1000.0f;
    float interval_max_ms = timing_interval_max_us / 1000.0f;
    float hotpath_avg_ms = (timing_hotpath_sum_us / (float)timing_samples) / 1000.0f;
    float hotpath_max_ms = timing_hotpath_max_us / 1000.0f;
    Serial.printf("[TIMING] samples=%lu loop avg/max %.2f/%.2f ms | hotpath avg/max %.2f/%.2f ms | MUX=%d ms\n",
        (unsigned long)timing_samples,
        interval_avg_ms, interval_max_ms,
        hotpath_avg_ms, hotpath_max_ms,
        MUX_SETTLE_DELAY);
    timing_zuruecksetzen();
}

void kalib_todo_ausgeben(uint8_t gyro, uint8_t accel, uint8_t mag) {
    bool hat_todo = false;
    Serial.print(" | TODO:");

    if (gyro < KALIB_SCHWELLE_GYRO) {
        Serial.print(" Gyro: komplett stillhalten;");
        hat_todo = true;
    }
    if (accel < KALIB_SCHWELLE_ACCEL) {
        Serial.print(" Accel: in andere feste Lage kippen/ablegen und kurz stillhalten;");
        hat_todo = true;
    }
    if (mag < KALIB_SCHWELLE_MAG) {
        Serial.print(" Mag: langsam 8er/um alle Achsen drehen, weg von Metall/USB;");
        hat_todo = true;
    }
    if (!hat_todo) {
        Serial.print(" 3/3/3 erreicht - jetzt ruhig halten fuer stabile Speicherung;");
    }
    Serial.println();
}

// Notaus: Toggle-Button nach GND mit Pull-Up.
// Jeder Tastendruck (fallende Flanke HIGH->LOW) toggelt notaus_aktiv.
void notaus_init() {
    pinMode(NOTAUS_PIN, INPUT_PULLUP);
    notaus_letzter_pegel = digitalRead(NOTAUS_PIN);
    notaus_pegel_stabil = notaus_letzter_pegel;
    notaus_aktiv = false;
    notaus_letzte_aenderung_ms = millis();
    Serial.println("[NOTAUS] Toggle-Button bereit — Betrieb freigegeben");
}

// Toggle-Button: Fallende Flanke (HIGH->LOW) toggelt Notaus
void notaus_lesen() {
    // Pin-Konfiguration sicherstellen (kann durch WiFi/I2C ueberschrieben werden)
    pinMode(NOTAUS_PIN, INPUT_PULLUP);
    bool pegel = digitalRead(NOTAUS_PIN);
    unsigned long jetzt = millis();

    if (pegel != notaus_letzter_pegel) {
        notaus_letzte_aenderung_ms = jetzt;
        notaus_letzter_pegel = pegel;
    }

    if ((jetzt - notaus_letzte_aenderung_ms) >= NOTAUS_ENTPRELL) {
        // Fallende Flanke erkennen: stabiler Pegel war HIGH, jetzt stabil LOW
        if (notaus_pegel_stabil == HIGH && pegel == LOW) {
            notaus_aktiv = !notaus_aktiv;
            if (notaus_aktiv) {
                Serial.println("[NOTAUS] *** NOTAUS AKTIVIERT (Toggle) ***");
            } else {
                Serial.println("[NOTAUS] Notaus DEAKTIVIERT (Toggle) — Betrieb freigegeben");
            }
        }
        notaus_pegel_stabil = pegel;
    }
}

void leds_init() {
    pinMode(LED_HAND, OUTPUT);
    pinMode(LED_UNTERARM, OUTPUT);
    pinMode(LED_OBERARM, OUTPUT);
    pinMode(LED_COMMS, OUTPUT);
    pinMode(LED_FAULT, OUTPUT);

    rgb.begin();
    rgb.setBrightness(30);
    rgb.clear();
    rgb.show();

    // Starttest: alle LEDs kurz an
    digitalWrite(LED_HAND, HIGH);
    digitalWrite(LED_UNTERARM, HIGH);
    digitalWrite(LED_OBERARM, HIGH);
    digitalWrite(LED_COMMS, HIGH);
    digitalWrite(LED_FAULT, HIGH);
    rgb.setPixelColor(0, rgb.Color(255, 0, 0));
    rgb.show();
    delay(300);
    digitalWrite(LED_HAND, LOW);
    digitalWrite(LED_UNTERARM, LOW);
    digitalWrite(LED_OBERARM, LOW);
    digitalWrite(LED_COMMS, LOW);
    digitalWrite(LED_FAULT, LOW);
    rgb.clear();
    rgb.show();
}

void leds_aktualisieren(const KalibStatus kalib[]) {
    // LED-Schema: AUS = OK, AN = Problem
    // IMU-LEDs: AUS wenn Sensor bereit UND kalibriert, AN wenn Problem
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        bool ok = sensor_bereit[i] && kalib[i].gyro >= KALIB_SCHWELLE_GYRO;
        digitalWrite(led_sensor_pin[i], ok ? LOW : HIGH);
    }

    // COMMS: AUS wenn letztes Senden erfolgreich
    digitalWrite(LED_COMMS, letzter_tx_ok ? LOW : HIGH);

    // FAULT: AN wenn Sensorausfall oder Flex-Fehler
    fehler_aktiv = !flex_bereit;
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        if (!sensor_bereit[i]) {
            fehler_aktiv = true;
            break;
        }
    }
    digitalWrite(LED_FAULT, fehler_aktiv ? HIGH : LOW);

    // RGB: Notaus = orange blinkend (hoechste Prio), Fehler = rot blinkend, OK = aus
    static bool blink = false;
    blink = !blink;
    if (notaus_aktiv) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 80, 0) : rgb.Color(0, 0, 0));
    } else if (fehler_aktiv) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 0, 0) : rgb.Color(0, 0, 0));
    } else {
        rgb.clear();
    }
    rgb.show();
}

void beiBesendung(const wifi_tx_info_t* tx_info, esp_now_send_status_t status) {
    // Bridge-Fehler ignorieren: COMMS-LED nur bei Receiver-Fehler ausschalten
#if BRIDGE_AKTIV
    if (memcmp(tx_info->des_addr, bridge_adresse, 6) == 0) {
        // Bridge-Send — Fehler sind unkritisch, nicht auf COMMS-LED auswirken
        return;
    }
#endif
    letzter_tx_ok = (status == ESP_NOW_SEND_SUCCESS);
    if (!letzter_tx_ok && kalib_einzeln < 0) {
        Serial.printf("[TX] FEHLER bei #%lu\n", sende_zaehler - 1);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    leds_init();
    notaus_init();

    prefs.begin("imu_kalib", false);

    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("=== Controller: IMU + ESP-NOW + LEDs ===");

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(50);
        if (!sensoren[i].begin()) {
            Serial.printf("FEHLER: Sensor %d nicht gefunden\n", i);
            sensor_bereit[i] = false;
        } else {
            sensoren[i].setExtCrystalUse(true);
            sensor_bereit[i] = true;
            Serial.printf("Sensor %d bereit\n", i);
        }
    }

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        kalib_laden(i);
        if (!kalib_gespeichert[i]) {
            kalib_status_lesen(i);
        }
    }
    mux_alle_schliessen();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // Echte MAC per eFuse (WiFi.macAddress() gibt 00:00:00:00:00:00 auf ESP32-S3)
    uint8_t eigene_mac[6];
    esp_efuse_mac_get_default(eigene_mac);
    Serial.printf("[INFO] Echte MAC (eFuse): %02X:%02X:%02X:%02X:%02X:%02X\n",
        eigene_mac[0], eigene_mac[1], eigene_mac[2],
        eigene_mac[3], eigene_mac[4], eigene_mac[5]);

    // Kanal explizit setzen fuer ESP-NOW
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    uint8_t primaer_kanal = 0;
    wifi_second_chan_t sekundaer_kanal;
    esp_wifi_get_channel(&primaer_kanal, &sekundaer_kanal);
    Serial.printf("[INFO] WiFi-Kanal: %d\n", primaer_kanal);

    Serial.printf("[INFO] Empfaenger-MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        empfaenger_adresse[0], empfaenger_adresse[1], empfaenger_adresse[2],
        empfaenger_adresse[3], empfaenger_adresse[4], empfaenger_adresse[5]);
#if BRIDGE_AKTIV
    Serial.printf("[INFO] Bridge-MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        bridge_adresse[0], bridge_adresse[1], bridge_adresse[2],
        bridge_adresse[3], bridge_adresse[4], bridge_adresse[5]);
#endif

    if (esp_now_init() != ESP_OK) {
        Serial.println("FEHLER: ESP-NOW init");
        return;
    }

    esp_now_register_send_cb(beiBesendung);

    esp_now_peer_info_t gegenstelle = {};
    memcpy(gegenstelle.peer_addr, empfaenger_adresse, 6);
    gegenstelle.channel = 0;
    gegenstelle.encrypt = false;

    if (esp_now_add_peer(&gegenstelle) != ESP_OK) {
        Serial.println("FEHLER: Peer konnte nicht hinzugefuegt werden");
        return;
    }

#if BRIDGE_AKTIV
    esp_now_peer_info_t bridge_peer = {};
    memcpy(bridge_peer.peer_addr, bridge_adresse, 6);
    bridge_peer.channel = 0;
    bridge_peer.encrypt = false;

    if (esp_now_add_peer(&bridge_peer) != ESP_OK) {
        Serial.println("[BRIDGE] WARNUNG: Bridge-Peer konnte nicht registriert werden");
    } else {
        bridge_registriert = true;
        Serial.println("[BRIDGE] Debug-Bridge als zweiter Peer registriert");
    }
#endif

    analogReadResolution(12);
    Serial.println("Bereit. Befehle: CAL0/CAL1/CAL2, STOP, RECAL, CALSTAT, FLEXRAW, FLEXSTAT, TIMING, DEBUGON, DEBUGOFF, DIAGON, DIAGOFF");
    Serial.println("[DEBUG] Periodischer Serial-Status ist aus. DEBUGON aktiviert ihn temporaer.");
    Serial.println("[DIAG] Hintergrund-Kalibrierstatus ist aus. DIAGON nur fuer Diagnose verwenden.");
}

// Poti/Flex-Ersatz: Werte nahe 0 deuten weiter auf Kabel-/Sensorproblem.
// Der 2-Draht-Poti darf am offenen Ende aber sauber bis an den ADC-Maximalwert
// laufen, deshalb ist die Obergrenze hier bewusst hoch.
#define FLEX_MIN_PLAUSIBEL  200
#define FLEX_MAX_PLAUSIBEL 4095

uint16_t flex_roh_lesen() {
    uint32_t summe = 0;
    for (uint8_t i = 0; i < 16; i++) summe += analogRead(FLEX_PIN);
    return summe / 16;
}

bool flex_rohwert_plausibel(uint16_t rohwert) {
    return rohwert >= FLEX_MIN_PLAUSIBEL && rohwert <= FLEX_MAX_PLAUSIBEL;
}

float flex_prozent_aus_rohwert(uint16_t rohwert) {
    float prozent = (float)(FLEX_GERADE - rohwert) / (FLEX_GERADE - FLEX_GEBOGEN) * 100.0f;
    if (prozent < 0.0f)   prozent = 0.0f;
    if (prozent > 100.0f) prozent = 100.0f;
    return prozent;
}

float flex_roh_filtern(uint16_t rohwert) {
    if (!flex_filter_initialisiert) {
        flex_roh_gefiltert = (float)rohwert;
        flex_filter_initialisiert = true;
        return flex_roh_gefiltert;
    }

    float delta = (float)rohwert - flex_roh_gefiltert;
    float abs_delta = (delta < 0.0f) ? -delta : delta;

    if (abs_delta < FLEX_RAW_DEADBAND) {
        return flex_roh_gefiltert;
    }

    flex_roh_gefiltert += delta * FLEX_FILTER_ALPHA;
    return flex_roh_gefiltert;
}

void flex_statistik_ausgeben() {
    const uint16_t samples = 200;
    uint16_t min_roh = 4095;
    uint16_t max_roh = 0;
    uint32_t sum_roh = 0;

    for (uint16_t i = 0; i < samples; i++) {
        uint16_t rohwert = flex_roh_lesen();
        if (rohwert < min_roh) min_roh = rohwert;
        if (rohwert > max_roh) max_roh = rohwert;
        sum_roh += rohwert;
        delay(5);
    }

    float avg_roh = (float)sum_roh / samples;
    Serial.printf(
        "[FLEX] ADC min=%u avg=%.1f max=%u | Prozent min=%.1f avg=%.1f max=%.1f | Gerade=%d Gebogen=%d\n",
        min_roh, avg_roh, max_roh,
        flex_prozent_aus_rohwert(min_roh),
        flex_prozent_aus_rohwert((uint16_t)(avg_roh + 0.5f)),
        flex_prozent_aus_rohwert(max_roh),
        FLEX_GERADE, FLEX_GEBOGEN
    );
}

float flex_lesen() {
    uint16_t rohwert = flex_roh_lesen();

    if (!flex_rohwert_plausibel(rohwert)) {
        if (flex_bereit) {
            flex_bereit = false;
            flex_filter_initialisiert = false;
            Serial.printf("[FAULT] Flex-Sensor unplausibel (ADC=%d)\n", rohwert);
        }
        return -1.0f;  // Signalwert: Sensor nicht verfuegbar
    }

    if (!flex_bereit) {
        flex_bereit = true;
        Serial.printf("[RECOVER] Flex-Sensor wieder plausibel (ADC=%d)\n", rohwert);
    }

    float rohwert_gefiltert = flex_roh_filtern(rohwert);
    return flex_prozent_aus_rohwert((uint16_t)(rohwert_gefiltert + 0.5f));
}

void serial_pruefen() {
    if (!Serial.available()) return;

    String eingabe = Serial.readStringUntil('\n');
    eingabe.trim();

    if (eingabe == "FLEXRAW") {
        uint16_t rohwert = flex_roh_lesen();
        Serial.printf(
            "[FLEX] ADC=%u plausibel=%s -> %.1f%% | Gerade=%d Gebogen=%d\n",
            rohwert,
            flex_rohwert_plausibel(rohwert) ? "ja" : "nein",
            flex_prozent_aus_rohwert(rohwert),
            FLEX_GERADE,
            FLEX_GEBOGEN
        );
    } else if (eingabe == "FLEXSTAT") {
        flex_statistik_ausgeben();
    } else if (eingabe == "CALSTAT") {
        kalib_status_alle_ausgeben();
    } else if (eingabe == "TIMING") {
        timing_ausgeben_und_reset();
    } else if (eingabe == "DEBUGON") {
        debug_status_aktiv = true;
        letzter_debug_ms = 0;
        timing_zuruecksetzen();
        Serial.println("[DEBUG] Periodischer Serial-Status AN");
    } else if (eingabe == "DEBUGOFF") {
        debug_status_aktiv = false;
        letzter_debug_ms = 0;
        timing_zuruecksetzen();
        Serial.println("[DEBUG] Periodischer Serial-Status AUS");
    } else if (eingabe == "DIAGON") {
        status_diag_aktiv = true;
        letzter_status_check_ms = 0;
        timing_zuruecksetzen();
        Serial.println("[DIAG] Hintergrund-Kalibrierstatus AN - kann Paketpausen erzeugen");
    } else if (eingabe == "DIAGOFF") {
        status_diag_aktiv = false;
        letzter_status_check_ms = 0;
        timing_zuruecksetzen();
        Serial.println("[DIAG] Hintergrund-Kalibrierstatus AUS");
    } else if (eingabe == "RECAL") {
        kalib_alle_loeschen();
        kalib_einzeln = -1;
    } else if (eingabe == "CAL0" || eingabe == "CAL1" || eingabe == "CAL2") {
        kalib_einzeln = eingabe.charAt(3) - '0';
        kalib_stabil_reset(kalib_einzeln);
        mux_kanal_oeffnen(kalib_einzeln);
        delay(10);
        Serial.printf("\n[KALIB] === Einzelkalibrierung S%d gestartet ===\n", kalib_einzeln);
        Serial.println("[KALIB] Gespeicherte Offsets bleiben erhalten, bis neue stabil gespeichert sind.");
        Serial.println("[KALIB] TODO Gyro : Sensor komplett stillhalten, bis G3 erreicht ist.");
        Serial.println("[KALIB] TODO Accel: Sensor in mehrere feste Lagen kippen/ablegen und kurz halten.");
        Serial.println("[KALIB] TODO Mag  : Langsame 8er und Rotation um alle Achsen, weg von Metall/USB.");
        Serial.println("[KALIB] Ziel: G>=3 A>=3 M>=3 dreimal stabil - dann wird gespeichert");
        Serial.println("[KALIB] STOP senden zum Abbrechen\n");
    } else if (eingabe == "STOP") {
        if (kalib_einzeln >= 0) {
            Serial.printf("[KALIB] Einzelkalibrierung S%d abgebrochen\n", kalib_einzeln);
            kalib_stabil_reset(kalib_einzeln);
            kalib_einzeln = -1;
            mux_alle_schliessen();
        }
    }
}

void loop_einzelkalibrierung() {
    uint8_t idx = kalib_einzeln;

    uint8_t sys, gyro, accel, mag;
    sensoren[idx].getCalibration(&sys, &gyro, &accel, &mag);

    sensors_event_t ereignis;
    sensoren[idx].getEvent(&ereignis);

    Serial.printf("[CAL S%d] H:%6.1f R:%6.1f P:%6.1f  [S%d G%d A%d M%d]",
        idx,
        ereignis.orientation.x, ereignis.orientation.y, ereignis.orientation.z,
        sys, gyro, accel, mag);

    // Kalibrierungs-LED blinken lassen
    static bool blink = false;
    blink = !blink;
    digitalWrite(led_sensor_pin[idx], blink ? HIGH : LOW);

    KalibStatus k = {sys, gyro, accel, mag};
    letzter_kalib_status[idx] = k;
    if (kalib_gut_genug(&k)) {
        if (kalib_stabil_genug(idx, &k)) {
            Serial.print("  3/3 | TODO: stabil genug - speichere Offsets\n");
            kalib_speichern(idx);
            Serial.printf("\n[KALIB] S%d FERTIG! Zurueck zum Normalbetrieb.\n", idx);
            digitalWrite(led_sensor_pin[idx], HIGH);
            kalib_einzeln = -1;
            mux_alle_schliessen();
        } else {
            Serial.printf("  3/3 stabil %d/%d", kalib_stabil_zaehler[idx], KALIB_SAVE_STABLE_COUNT);
            kalib_todo_ausgeben(gyro, accel, mag);
        }
    } else {
        kalib_stabil_reset(idx);
        uint8_t ok = 0;
        if (gyro >= KALIB_SCHWELLE_GYRO) ok++;
        if (accel >= KALIB_SCHWELLE_ACCEL) ok++;
        if (mag >= KALIB_SCHWELLE_MAG) ok++;
        Serial.printf("  %d/3", ok);
        kalib_todo_ausgeben(gyro, accel, mag);
    }

    delay(500);
}

void controller_status_langsam_pruefen() {
    if (!status_diag_aktiv) {
        return;
    }

    unsigned long jetzt = millis();
    if (jetzt - letzter_status_check_ms < STATUS_CHECK_INTERVALL) {
        return;
    }
    letzter_status_check_ms = jetzt;

    uint8_t i = status_check_sensor;
    status_check_sensor = (status_check_sensor + 1) % ANZAHL_SENSOREN;

    mux_kanal_oeffnen(i);
    delay(MUX_SETTLE_DELAY);

    uint8_t sys_status = 0;
    sensoren[i].getSystemStatus(&sys_status, NULL, NULL);
    if (sys_status == 0) {
        if (sensor_bereit[i]) {
            sensor_bereit[i] = false;
            letzter_kalib_status[i] = {0, 0, 0, 0};
            kalib_stabil_reset(i);
            Serial.printf("[FAULT] Sensor %d ausgefallen!\n", i);
        }
        mux_alle_schliessen();
        return;
    }

    if (!sensor_bereit[i]) {
        if (sensoren[i].begin()) {
            sensoren[i].setExtCrystalUse(true);
            sensor_bereit[i] = true;
            kalib_laden(i);
            Serial.printf("[RECOVER] Sensor %d wiederhergestellt\n", i);
        } else {
            letzter_kalib_status[i] = {0, 0, 0, 0};
            kalib_stabil_reset(i);
            mux_alle_schliessen();
            return;
        }
    }

    if (!kalib_status_lesen(i)) {
        mux_alle_schliessen();
        return;
    }

    KalibStatus k = letzter_kalib_status[i];

    if (!kalib_gespeichert[i] && kalib_stabil_genug(i, &k)) {
        kalib_speichern(i);
    }

    mux_alle_schliessen();
}

void loop_normal() {
    unsigned long loop_start_us = micros();
    uint32_t loop_interval_us = 0;
    if (letzter_loop_start_us != 0) {
        loop_interval_us = (uint32_t)(loop_start_us - letzter_loop_start_us);
    }
    letzter_loop_start_us = loop_start_us;

    notaus_lesen();

    ImuPaket paket = {};
    paket.protokoll_version = PROTOKOLL_VERSION;
    paket.flags = 0;
    if (notaus_aktiv) paket.flags |= FLAG_NOTAUS;
    paket.zaehler = sende_zaehler++;

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(MUX_SETTLE_DELAY);

        paket.kalib[i] = letzter_kalib_status[i];
        if (!sensor_bereit[i]) {
            paket.sensoren[i] = letzter_gueltiger_sensor_vorhanden[i]
                ? letzter_gueltiger_sensor[i]
                : SensorDaten{0.0f, 0.0f, 0.0f};
            continue;
        }

        sensors_event_t ereignis;
        sensoren[i].getEvent(&ereignis);
        SensorDaten gelesen = {
            ereignis.orientation.x,
            ereignis.orientation.y,
            ereignis.orientation.z
        };
        paket.sensoren[i] = sensorwert_uebernehmen_oder_halten(i, gelesen);
    }
    mux_alle_schliessen();

    leds_aktualisieren(paket.kalib);

    paket.flex_prozent = flex_lesen();
    paket.pruefsumme = pruefsumme_berechnen(&paket);

    esp_err_t rc_empf = esp_now_send(empfaenger_adresse, (uint8_t*)&paket, sizeof(paket));

#if BRIDGE_AKTIV
    esp_err_t rc_bridge = ESP_OK;
    if (bridge_registriert) {
        rc_bridge = esp_now_send(bridge_adresse, (uint8_t*)&paket, sizeof(paket));
    }
#endif
    uint32_t hotpath_us = (uint32_t)(micros() - loop_start_us);
    timing_erfassen(loop_interval_us, hotpath_us);

    // Periodische Debug-Ausgabe nur bei Bedarf: Serial-Dumps bremsen den
    // Livepfad sichtbar aus und bleiben deshalb im Normalbetrieb deaktiviert.
    unsigned long jetzt = millis();
    if (debug_status_aktiv && jetzt - letzter_debug_ms >= DEBUG_INTERVALL) {
        letzter_debug_ms = jetzt;
        Serial.printf("\n--- Status #%lu (%.1fs) ---\n", paket.zaehler, jetzt / 1000.0f);
        for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
            Serial.printf("  S%d | H:%6.1f R:%6.1f P:%6.1f  [S%d G%d A%d M%d] %s\n", i,
                paket.sensoren[i].heading, paket.sensoren[i].roll, paket.sensoren[i].pitch,
                paket.kalib[i].sys, paket.kalib[i].gyro,
                paket.kalib[i].accel, paket.kalib[i].mag,
                sensor_bereit[i] ? "OK" : "FEHLT");
        }
        Serial.printf("  Flex: %5.1f%%  TX-Recv: %s", paket.flex_prozent,
            (rc_empf == ESP_OK) ? "OK" : "FEHLER");
#if BRIDGE_AKTIV
        Serial.printf("  TX-Bridge: %s", (rc_bridge == ESP_OK) ? "OK" : "FEHLER");
#endif
        Serial.printf("  GPIO%d=%d notaus=%d", NOTAUS_PIN, digitalRead(NOTAUS_PIN), notaus_aktiv);
        if (notaus_aktiv) Serial.print("  *** NOTAUS ***");
        Serial.println();
        Serial.printf("  NullHold: S0=%lu S1=%lu S2=%lu\n",
            (unsigned long)sensor_nullframe_hold_zaehler[0],
            (unsigned long)sensor_nullframe_hold_zaehler[1],
            (unsigned long)sensor_nullframe_hold_zaehler[2]);

        timing_ausgeben_und_reset();
    }

    controller_status_langsam_pruefen();
    delay(SENDE_INTERVALL);
}

void loop() {
    serial_pruefen();

    if (kalib_einzeln >= 0) {
        loop_einzelkalibrierung();
    } else {
        loop_normal();
    }
}
