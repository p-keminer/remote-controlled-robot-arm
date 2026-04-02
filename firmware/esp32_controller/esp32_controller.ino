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
// LED-Debugging (alle mit 100 Ohm Vorwiderstand):
//   GPIO4  Gruen  — Hand/Wrist-IMU (S2) kalibriert
//   GPIO5  Gelb   — Unterarm-IMU (S1) kalibriert
//   GPIO6  Rot    — Oberarm-IMU (S0) kalibriert
//   GPIO7  Blau   — COMMS (blinkt bei erfolgreichem Senden)
//   GPIO10 Weiss  — FAULT (leuchtet bei Fehler)
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
#define FLEX_GERADE       1108
#define FLEX_GEBOGEN      940

#define KALIB_SCHWELLE_GYRO  3
#define KALIB_SCHWELLE_ACCEL 2
#define KALIB_SCHWELLE_MAG   2

// LED-Pins: Ampelsystem Hand(gruen)->Unterarm(gelb)->Oberarm(rot)
#define LED_HAND          4   // Gruen  — S2 Hand/Wrist
#define LED_UNTERARM      5   // Gelb   — S1 Unterarm
#define LED_OBERARM       6   // Rot    — S0 Oberarm
#define LED_COMMS         7   // Blau   — Kommunikation
#define LED_FAULT        10   // Weiss  — Fehler
#define RGB_PIN          48   // Interne RGB-LED
#define RGB_ANZAHL        1

// Flags-Bitfeld (ImuPaket v4)
#define FLAG_NOTAUS       (1 << 0)   // Bit 0: Notaus aktiv
// Bits 1-7: reserviert fuer spaetere Erweiterung (Sensor-Gueltigkeit, Auth, etc.)

// Zuordnung: LED-Pin pro Sensor-Index (S0=Oberarm, S1=Unterarm, S2=Hand)
Adafruit_NeoPixel rgb(RGB_ANZAHL, RGB_PIN, NEO_GRB + NEO_KHZ800);

static const uint8_t led_sensor_pin[ANZAHL_SENSOREN] = {
    LED_OBERARM,    // S0
    LED_UNTERARM,   // S1
    LED_HAND        // S2
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
static bool sensor_bereit[ANZAHL_SENSOREN] = {false, false, false};
static bool letzter_tx_ok = false;
static bool fehler_aktiv = false;
static bool flex_bereit = true;
#if BRIDGE_AKTIV
static bool bridge_registriert = false;
#endif

// Notaus-Zustand (Toggle-Button, entprellt)
static bool notaus_aktiv = false;
static bool notaus_letzter_pegel = HIGH;       // Pull-Up: Ruhezustand = HIGH
static bool notaus_pegel_stabil = HIGH;        // letzter stabiler Pegel
static unsigned long notaus_letzte_aenderung_ms = 0;

// Debug-Ausgabe Intervall
#define DEBUG_INTERVALL 2000  // ms
static unsigned long letzter_debug_ms = 0;

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

void kalib_laden(uint8_t idx) {
    char key[8];
    snprintf(key, sizeof(key), "cal%d", idx);

    adafruit_bno055_offsets_t offsets;
    size_t len = prefs.getBytes(key, &offsets, sizeof(offsets));
    if (len == sizeof(offsets)) {
        mux_kanal_oeffnen(idx);
        delay(10);
        sensoren[idx].setSensorOffsets(offsets);
        kalib_gespeichert[idx] = true;
        Serial.printf("[KALIB] S%d: Offsets aus NVS geladen\n", idx);
    } else {
        Serial.printf("[KALIB] S%d: Keine gespeicherten Offsets\n", idx);
    }
}

void kalib_speichern(uint8_t idx) {
    char key[8];
    snprintf(key, sizeof(key), "cal%d", idx);

    adafruit_bno055_offsets_t offsets;
    mux_kanal_oeffnen(idx);
    delay(10);
    sensoren[idx].getSensorOffsets(offsets);
    prefs.putBytes(key, &offsets, sizeof(offsets));
    kalib_gespeichert[idx] = true;
    Serial.printf("[KALIB] S%d: Offsets in NVS gespeichert!\n", idx);
}

void kalib_alle_loeschen() {
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        char key[8];
        snprintf(key, sizeof(key), "cal%d", i);
        prefs.remove(key);
        kalib_gespeichert[i] = false;
    }
    Serial.println("[KALIB] Alle Offsets geloescht");
}

bool kalib_gut_genug(const KalibStatus* k) {
    return k->gyro >= KALIB_SCHWELLE_GYRO
        && k->accel >= KALIB_SCHWELLE_ACCEL
        && k->mag >= KALIB_SCHWELLE_MAG;
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
    Serial.println("Bereit. Befehle: CAL0/CAL1/CAL2, STOP, RECAL");
}

// Flex-Sensor: plausible ADC-Werte liegen zwischen FLEX_GEBOGEN und FLEX_GERADE
// mit Toleranz. Werte nahe 0 oder 4095 deuten auf abgezogenen Sensor.
#define FLEX_MIN_PLAUSIBEL  200
#define FLEX_MAX_PLAUSIBEL 3800

float flex_lesen() {
    uint32_t summe = 0;
    for (uint8_t i = 0; i < 16; i++) summe += analogRead(FLEX_PIN);
    uint16_t rohwert = summe / 16;

    if (rohwert < FLEX_MIN_PLAUSIBEL || rohwert > FLEX_MAX_PLAUSIBEL) {
        if (flex_bereit) {
            flex_bereit = false;
            Serial.printf("[FAULT] Flex-Sensor unplausibel (ADC=%d)\n", rohwert);
        }
        return -1.0f;  // Signalwert: Sensor nicht verfuegbar
    }

    if (!flex_bereit) {
        flex_bereit = true;
        Serial.printf("[RECOVER] Flex-Sensor wieder plausibel (ADC=%d)\n", rohwert);
    }

    float prozent = (float)(FLEX_GERADE - rohwert) / (FLEX_GERADE - FLEX_GEBOGEN) * 100.0f;
    if (prozent < 0.0f)   prozent = 0.0f;
    if (prozent > 100.0f) prozent = 100.0f;
    return prozent;
}

void serial_pruefen() {
    if (!Serial.available()) return;

    String eingabe = Serial.readStringUntil('\n');
    eingabe.trim();

    if (eingabe == "RECAL") {
        kalib_alle_loeschen();
        kalib_einzeln = -1;
    } else if (eingabe == "CAL0" || eingabe == "CAL1" || eingabe == "CAL2") {
        kalib_einzeln = eingabe.charAt(3) - '0';
        kalib_gespeichert[kalib_einzeln] = false;
        char key[8];
        snprintf(key, sizeof(key), "cal%d", kalib_einzeln);
        prefs.remove(key);
        mux_kanal_oeffnen(kalib_einzeln);
        delay(10);
        Serial.printf("\n[KALIB] === Einzelkalibrierung S%d gestartet ===\n", kalib_einzeln);
        Serial.println("[KALIB] Sensor bewegen: 8er-Form (Mag), still halten (Gyro),");
        Serial.println("[KALIB] verschiedene Seiten ablegen (Accel)");
        Serial.println("[KALIB] Ziel: G>=3 A>=2 M>=2 — wird automatisch gespeichert");
        Serial.println("[KALIB] STOP senden zum Abbrechen\n");
    } else if (eingabe == "STOP") {
        if (kalib_einzeln >= 0) {
            Serial.printf("[KALIB] Einzelkalibrierung S%d abgebrochen\n", kalib_einzeln);
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
    if (kalib_gut_genug(&k)) {
        kalib_speichern(idx);
        Serial.printf("\n[KALIB] S%d FERTIG! Zurueck zum Normalbetrieb.\n", idx);
        digitalWrite(led_sensor_pin[idx], HIGH);
        kalib_einzeln = -1;
        mux_alle_schliessen();
    } else {
        uint8_t ok = 0;
        if (gyro >= KALIB_SCHWELLE_GYRO) ok++;
        if (accel >= KALIB_SCHWELLE_ACCEL) ok++;
        if (mag >= KALIB_SCHWELLE_MAG) ok++;
        Serial.printf("  %d/3\n", ok);
    }

    delay(500);
}

static uint8_t status_check_zaehler = 0;

void loop_normal() {
    notaus_lesen();

    ImuPaket paket;
    paket.protokoll_version = PROTOKOLL_VERSION;
    paket.flags = 0;
    if (notaus_aktiv) paket.flags |= FLAG_NOTAUS;
    paket.zaehler = sende_zaehler++;

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(10);

        // Live-Pruefung: nur alle 10 Zyklen (spart ~30ms pro Durchlauf)
        bool status_pruefen = (status_check_zaehler == 0);
        if (status_pruefen) {
            uint8_t sys_status = 0;
            sensoren[i].getSystemStatus(&sys_status, NULL, NULL);
            if (sys_status == 0) {
            // Sensor antwortet nicht — als ausgefallen markieren
            if (sensor_bereit[i]) {
                sensor_bereit[i] = false;
                Serial.printf("[FAULT] Sensor %d ausgefallen!\n", i);
            }
            paket.sensoren[i] = {0, 0, 0};
            paket.kalib[i] = {0, 0, 0, 0};
            continue;
        } else if (!sensor_bereit[i]) {
            // Sensor wieder da — neu initialisieren
            if (sensoren[i].begin()) {
                sensoren[i].setExtCrystalUse(true);
                sensor_bereit[i] = true;
                kalib_laden(i);
                Serial.printf("[RECOVER] Sensor %d wiederhergestellt\n", i);
            }
        }

        }  // if (status_pruefen)

        sensors_event_t ereignis;
        sensoren[i].getEvent(&ereignis);
        paket.sensoren[i].heading = ereignis.orientation.x;
        paket.sensoren[i].roll    = ereignis.orientation.y;
        paket.sensoren[i].pitch   = ereignis.orientation.z;
        // Kalibrierung nur alle 20 Zyklen abfragen (aendert sich langsam)
        if (status_check_zaehler == 0) {
            sensoren[i].getCalibration(
                &paket.kalib[i].sys, &paket.kalib[i].gyro,
                &paket.kalib[i].accel, &paket.kalib[i].mag);

            if (!kalib_gespeichert[i] && kalib_gut_genug(&paket.kalib[i])) {
                kalib_speichern(i);
            }
        }
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

    // Periodische Debug-Ausgabe alle 2 Sekunden
    unsigned long jetzt = millis();
    if (jetzt - letzter_debug_ms >= DEBUG_INTERVALL) {
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
    }

    status_check_zaehler = (status_check_zaehler + 1) % 20;
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
