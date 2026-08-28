#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Adafruit_NeoPixel.h>

#include "i2c_frame.h"

#ifndef RECEIVER_MODE_TEST_SWEEP
#define RECEIVER_MODE_TEST_SWEEP 1
#endif

#ifndef RECEIVER_MODE_LIVE_I2C
#define RECEIVER_MODE_LIVE_I2C 0
#endif

#if RECEIVER_MODE_TEST_SWEEP && RECEIVER_MODE_LIVE_I2C
#error "Nur ein Receiver-I2C-Modus darf aktiv sein."
#endif

#if RECEIVER_MODE_TEST_SWEEP
#include "test_sweep_i2c.h"
#endif

// Receiver-I2C: IMU-Pakete empfangen, validieren und fuer Arduino-I2C vorbereiten
// Prueft: Protokollversion, Pruefsumme, Frame-Frische
//
// LED-Debugging (invertiert: aus = OK, blinken = Problem):
//   GPIO4  Gruen  — blinkt wenn I2C-Problem (spaeter)
//   GPIO5  Blau   — blinkt wenn ESP-NOW Timeout
//   GPIO48 RGB    — rot blinkend bei FAULT (Pruefsumme, Version)

#define PROTOKOLL_VERSION 4
#define ANZAHL_SENSOREN   3

#define LED_I2C           4   // Gruen — I2C-Problem
#define LED_LINK          5   // Blau  — ESP-NOW Timeout
#define RGB_PIN          48   // Interne RGB-LED
#define RGB_ANZAHL        1

#define EMPFANGS_TIMEOUT  2000  // ms ohne Paket = Verbindung verloren
#define LIVE_I2C_FRAME_INTERVALL_MS 20   // 50 Hz Ziel-Takt fuer spaeteren Live-Pfad
#define LIVE_I2C_STALE_MS 250            // ohne frisches Paket: Neutral-Frame
#define LIVE_I2C_MAX_BYTE_STEP 2         // Byte-Slew-Limiter pro 20ms-Frame
#define LIVE_I2C_ELBOW_MAX_BYTE_STEP 8   // Ellbogen passend zum Arduino-Slew direkter nachfuehren
#define LIVE_I2C_WRIST_MAX_BYTE_STEP 8   // Wrist folgt direkter; Arduino glaettet final in Grad
#define LIVE_I2C_GRIPPER_MAX_BYTE_STEP 12 // Greifer darf direkter reagieren als Armachsen
#define LIVE_I2C_NEUTRAL_BYTE 128
#define RECEIVER_SERIAL_DEBUG_INTERVALL_MS 500

#ifndef RECEIVER_LIVE_ENABLE_BASIS
#define RECEIVER_LIVE_ENABLE_BASIS 0
#endif
#ifndef RECEIVER_LIVE_ENABLE_SHOULDER
#define RECEIVER_LIVE_ENABLE_SHOULDER 0
#endif
#ifndef RECEIVER_LIVE_ENABLE_ELBOW
#define RECEIVER_LIVE_ENABLE_ELBOW 0
#endif
#ifndef RECEIVER_LIVE_ENABLE_WRIST
#define RECEIVER_LIVE_ENABLE_WRIST 0
#endif
#ifndef RECEIVER_LIVE_ENABLE_GRIPPER
#define RECEIVER_LIVE_ENABLE_GRIPPER 0
#endif
#ifndef RECEIVER_I2C_SEND_ENABLED
#define RECEIVER_I2C_SEND_ENABLED 1
#endif

// Flags-Bitfeld (ImuPaket v4)
#define FLAG_NOTAUS       (1 << 0)   // Bit 0: Notaus aktiv

Adafruit_NeoPixel rgb(RGB_ANZAHL, RGB_PIN, NEO_GRB + NEO_KHZ800);

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
    uint8_t     flags;               // Bitfeld: Bit 0 = Notaus
    uint8_t     protokoll_version;   // 4
    uint8_t     pruefsumme;
} ImuPaket;

#include "live_i2c_mapping.h"

static uint32_t letzter_zaehler = UINT32_MAX;
static unsigned long letzter_empfang_ms = 1;  // >0 damit Timeout ab Boot greift
static bool fehler_flag = false;
static bool notaus_empfangen = false;
static volatile bool neues_gueltiges_paket = false;
static ImuPaket letztes_gueltiges_paket = {};
static portMUX_TYPE paket_mutex = portMUX_INITIALIZER_UNLOCKED;
static unsigned long letzter_live_i2c_ms = 0;
static uint8_t live_basis = LIVE_I2C_NEUTRAL_BYTE;
static uint8_t live_schulter = LIVE_I2C_NEUTRAL_BYTE;
static uint8_t live_ellbogen = LIVE_I2C_NEUTRAL_BYTE;
static uint8_t live_handgelenk = LIVE_I2C_NEUTRAL_BYTE;
static uint8_t live_greifer = LIVE_I2C_NEUTRAL_BYTE;
static unsigned long letzter_debug_print_ms = 0;
static unsigned long letzter_live_i2c_debug_ms = 0;
static float letzter_live_i2c_wrist_raw_deg = 0.0f;
static float letzter_live_i2c_wrist_deg = 0.0f;
static uint32_t letzter_debug_zaehler = UINT32_MAX;
static bool letzter_debug_notaus = false;

uint8_t pruefsumme_berechnen(const ImuPaket* paket) {
    const uint8_t* bytes = (const uint8_t*)paket;
    uint8_t summe = 0;
    for (size_t i = 0; i < sizeof(ImuPaket) - 1; i++) {
        summe ^= bytes[i];
    }
    return summe;
}

void beiEmpfang(const esp_now_recv_info_t* empfangs_info, const uint8_t* daten, int laenge) {
    if (laenge != sizeof(ImuPaket)) {
        fehler_flag = true;
        return;
    }

    ImuPaket paket;
    memcpy(&paket, daten, sizeof(paket));

    if (paket.protokoll_version != PROTOKOLL_VERSION) {
        fehler_flag = true;
        return;
    }

    if (paket.pruefsumme != pruefsumme_berechnen(&paket)) {
        fehler_flag = true;
        return;
    }

    if (paket.zaehler <= letzter_zaehler && letzter_zaehler != UINT32_MAX) {
        return;
    }
    letzter_zaehler = paket.zaehler;
    letzter_empfang_ms = millis();
    fehler_flag = false;

    portENTER_CRITICAL(&paket_mutex);
    memcpy(&letztes_gueltiges_paket, &paket, sizeof(paket));
    neues_gueltiges_paket = true;
    portEXIT_CRITICAL(&paket_mutex);

    // Notaus-Flag auswerten
    bool neuer_notaus = (paket.flags & FLAG_NOTAUS) != 0;
    notaus_empfangen = neuer_notaus;
}

void debug_paket_ausgeben(unsigned long jetzt) {
    ImuPaket paket;
    if (!letztes_paket_kopieren(&paket)) return;

    bool notaus_aktiv = (paket.flags & FLAG_NOTAUS) != 0;
    bool notaus_geaendert = notaus_aktiv != letzter_debug_notaus;
    bool neues_frame = paket.zaehler != letzter_debug_zaehler;
    bool intervall_abgelaufen = (jetzt - letzter_debug_print_ms) >= RECEIVER_SERIAL_DEBUG_INTERVALL_MS;

    if (!notaus_geaendert && (!neues_frame || !intervall_abgelaufen)) return;

    letzter_debug_print_ms = jetzt;
    letzter_debug_zaehler = paket.zaehler;

    if (notaus_geaendert) {
        letzter_debug_notaus = notaus_aktiv;
        if (notaus_aktiv) {
            Serial.println("[NOTAUS] *** NOTAUS vom Controller empfangen - Bewegung gesperrt ***");
        } else {
            Serial.println("[NOTAUS] Controller-Notaus aufgehoben - Betrieb freigegeben");
        }
    }

    Serial.printf("#%lu", paket.zaehler);
    if (notaus_aktiv) Serial.print(" [NOTAUS]");
    Serial.println();
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        Serial.printf("  S%d | H:%6.1f  R:%6.1f  P:%6.1f  [S%d G%d A%d M%d]\n", i,
            paket.sensoren[i].heading,
            paket.sensoren[i].roll,
            paket.sensoren[i].pitch,
            paket.kalib[i].sys, paket.kalib[i].gyro,
            paket.kalib[i].accel, paket.kalib[i].mag);
    }
    Serial.printf("  Greifer: %5.1f%%\n", paket.flex_prozent);
}

bool letztes_paket_kopieren(ImuPaket* ziel) {
    if (ziel == NULL) return false;

    portENTER_CRITICAL(&paket_mutex);
    bool verfuegbar = neues_gueltiges_paket;
    if (verfuegbar) {
        memcpy(ziel, &letztes_gueltiges_paket, sizeof(ImuPaket));
    }
    portEXIT_CRITICAL(&paket_mutex);

    return verfuegbar;
}

uint8_t slew_byte(uint8_t aktuell, uint8_t ziel) {
    int delta = (int)ziel - (int)aktuell;
    if (delta > LIVE_I2C_MAX_BYTE_STEP) return aktuell + LIVE_I2C_MAX_BYTE_STEP;
    if (delta < -LIVE_I2C_MAX_BYTE_STEP) return aktuell - LIVE_I2C_MAX_BYTE_STEP;
    return ziel;
}

uint8_t slew_byte_mit_limit(uint8_t aktuell, uint8_t ziel, uint8_t max_step) {
    int delta = (int)ziel - (int)aktuell;
    if (delta > (int)max_step) return aktuell + max_step;
    if (delta < -(int)max_step) return aktuell - max_step;
    return ziel;
}

uint8_t live_achse_anwenden(uint8_t aktuell, uint8_t ziel, bool freigegeben) {
    return slew_byte(aktuell, freigegeben ? ziel : LIVE_I2C_NEUTRAL_BYTE);
}

uint8_t live_greifer_anwenden(uint8_t aktuell, uint8_t ziel, bool freigegeben) {
    return slew_byte_mit_limit(
        aktuell,
        freigegeben ? ziel : LIVE_I2C_NEUTRAL_BYTE,
        LIVE_I2C_GRIPPER_MAX_BYTE_STEP
    );
}

uint8_t live_wrist_anwenden(uint8_t aktuell, uint8_t ziel, bool freigegeben) {
    return slew_byte_mit_limit(
        aktuell,
        freigegeben ? ziel : LIVE_I2C_NEUTRAL_BYTE,
        LIVE_I2C_WRIST_MAX_BYTE_STEP
    );
}

uint8_t live_ellbogen_anwenden(uint8_t aktuell, uint8_t ziel, bool freigegeben) {
    return slew_byte_mit_limit(
        aktuell,
        freigegeben ? ziel : LIVE_I2C_NEUTRAL_BYTE,
        LIVE_I2C_ELBOW_MAX_BYTE_STEP
    );
}

void receiver_i2c_frame_senden(uint8_t flags, uint8_t basis, uint8_t schulter,
                               uint8_t ellbogen, uint8_t handgelenk, uint8_t greifer) {
#if RECEIVER_I2C_SEND_ENABLED
    i2c_frame_senden(flags, basis, schulter, ellbogen, handgelenk, greifer);
#else
    (void)flags;
    (void)basis;
    (void)schulter;
    (void)ellbogen;
    (void)handgelenk;
    (void)greifer;
#endif
}

void debug_live_i2c_frame(unsigned long jetzt, const char* status, uint8_t flags,
                          uint8_t basis, uint8_t schulter, uint8_t ellbogen,
                          uint8_t handgelenk, uint8_t greifer) {
    if ((jetzt - letzter_live_i2c_debug_ms) < RECEIVER_SERIAL_DEBUG_INTERVALL_MS) return;
    letzter_live_i2c_debug_ms = jetzt;

    Serial.printf("[LIVE-I2C] %s flags=0x%02X active[B%d S%d E%d W%d G%d] send[B=%u S=%u E=%u W=%u G=%u] I2C=%s\n",
        status,
        flags,
        RECEIVER_LIVE_ENABLE_BASIS,
        RECEIVER_LIVE_ENABLE_SHOULDER,
        RECEIVER_LIVE_ENABLE_ELBOW,
        RECEIVER_LIVE_ENABLE_WRIST,
        RECEIVER_LIVE_ENABLE_GRIPPER,
        basis, schulter, ellbogen, handgelenk, greifer,
        RECEIVER_I2C_SEND_ENABLED ? "on" : "off");
    Serial.printf("[LIVE-I2C] wristRaw=%7.2f wristDeg=%7.2f\n",
        letzter_live_i2c_wrist_raw_deg,
        letzter_live_i2c_wrist_deg);
    Serial.printf("[LIVE-I2C] wristOffset=%7.2f wristZero=%d\n",
        live_wrist_center_offset_deg,
        live_wrist_zero_done ? 1 : 0);
}

void serial_commands_update() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "WRISTZERO") {
        live_wrist_zero_anfordern();
        Serial.println("[CMD] WRISTZERO angefordert: naechstes gueltiges Paket wird Wrist-Mitte.");
    } else if (cmd == "WRISTSTAT") {
        Serial.printf("[CMD] WRISTSTAT raw=%7.2f deg=%7.2f offset=%7.2f zero=%d\n",
            letzter_live_i2c_wrist_raw_deg,
            letzter_live_i2c_wrist_deg,
            live_wrist_center_offset_deg,
            live_wrist_zero_done ? 1 : 0);
    } else if (cmd.length() > 0) {
        Serial.printf("[CMD] Unbekannt: %s\n", cmd.c_str());
    }
}

void live_i2c_update() {
    unsigned long jetzt = millis();
    if (jetzt - letzter_live_i2c_ms < LIVE_I2C_FRAME_INTERVALL_MS) return;
    letzter_live_i2c_ms = jetzt;

    ImuPaket paket;
    bool paket_verfuegbar = letztes_paket_kopieren(&paket);
    bool paket_alt = (letzter_empfang_ms == 0) || ((jetzt - letzter_empfang_ms) > LIVE_I2C_STALE_MS);

    if (!paket_verfuegbar || paket_alt) {
        receiver_i2c_frame_senden(FRAME_FLAG_NEUTRAL, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        debug_live_i2c_frame(jetzt, "neutral/stale", FRAME_FLAG_NEUTRAL, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        return;
    }

    if (paket.flags & FLAG_NOTAUS) {
        receiver_i2c_frame_senden(FRAME_FLAG_ESTOP, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        debug_live_i2c_frame(jetzt, "estop", FRAME_FLAG_ESTOP, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        return;
    }

    LiveI2cZiele ziele;
    if (!live_i2c_mapping_berechnen(&paket, &ziele)) {
        receiver_i2c_frame_senden(FRAME_FLAG_NEUTRAL, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        debug_live_i2c_frame(jetzt, "neutral/mapping", FRAME_FLAG_NEUTRAL, LIVE_I2C_NEUTRAL_BYTE,
            LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE, LIVE_I2C_NEUTRAL_BYTE);
        return;
    }
    letzter_live_i2c_wrist_raw_deg = ziele.handgelenk_raw_deg;
    letzter_live_i2c_wrist_deg = ziele.handgelenk_deg;

    live_basis = live_achse_anwenden(live_basis, ziele.basis, RECEIVER_LIVE_ENABLE_BASIS);
    live_schulter = live_achse_anwenden(live_schulter, ziele.schulter, RECEIVER_LIVE_ENABLE_SHOULDER);
    live_ellbogen = live_ellbogen_anwenden(live_ellbogen, ziele.ellbogen, RECEIVER_LIVE_ENABLE_ELBOW);
    live_handgelenk = live_wrist_anwenden(
        live_handgelenk,
        ziele.handgelenk,
        RECEIVER_LIVE_ENABLE_WRIST
    );
    live_greifer = live_greifer_anwenden(live_greifer, ziele.greifer, RECEIVER_LIVE_ENABLE_GRIPPER);

    receiver_i2c_frame_senden(
        FRAME_FLAG_VALID,
        live_basis,
        live_schulter,
        live_ellbogen,
        live_handgelenk,
        live_greifer
    );
    debug_live_i2c_frame(jetzt, "valid", FRAME_FLAG_VALID, live_basis, live_schulter,
        live_ellbogen, live_handgelenk, live_greifer);
}

void leds_init() {
    pinMode(LED_LINK, OUTPUT);
    pinMode(LED_I2C, OUTPUT);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_I2C, LOW);

    rgb.begin();
    rgb.setBrightness(30);
    rgb.clear();
    rgb.show();

    // Starttest: alle LEDs kurz an
    digitalWrite(LED_LINK, HIGH);
    digitalWrite(LED_I2C, HIGH);
    rgb.setPixelColor(0, rgb.Color(255, 0, 0));
    rgb.show();
    delay(300);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_I2C, LOW);
    rgb.clear();
    rgb.show();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    leds_init();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    // Kanal 1 = Router-Kanal, muss gleich sein wie Controller und Bridge
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    Serial.println("=== Receiver: IMU-Daten (Kanal 1) ===");
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    if (esp_now_init() != ESP_OK) {
        Serial.println("FEHLER: ESP-NOW init");
        return;
    }

    esp_now_register_recv_cb(beiEmpfang);

    // I2C-Master an Arduino-Slave (SDA=GPIO13, SCL=GPIO14)
    i2c_frame_init();
#if RECEIVER_MODE_TEST_SWEEP
    Serial.println("I2C Master aktiv (Default: Test-Sweep an Arduino Slave 0x42).");
#elif RECEIVER_MODE_LIVE_I2C
    Serial.println("I2C Master aktiv (Live-I2C vorbereitet: IMU -> Servo-Zielbytes).");
    Serial.printf("Live-Achsenfreigabe: Basis=%d Schulter=%d Ellbogen=%d Wrist=%d Greifer=%d | I2C-Send=%d\n",
        RECEIVER_LIVE_ENABLE_BASIS,
        RECEIVER_LIVE_ENABLE_SHOULDER,
        RECEIVER_LIVE_ENABLE_ELBOW,
        RECEIVER_LIVE_ENABLE_WRIST,
        RECEIVER_LIVE_ENABLE_GRIPPER,
        RECEIVER_I2C_SEND_ENABLED);
#else
    Serial.println("I2C Master aktiv (kein Ausgabe-Modus aktiv).");
#endif
    Serial.println("Bereit. LEDs aus = alles OK.");
}

void loop() {
    unsigned long jetzt = millis();
    bool blink = (jetzt / 500) % 2;  // 1Hz Blinktakt

    // ESP-NOW Timeout: Blau blinkt
    bool espnow_timeout = (letzter_empfang_ms > 0 && (jetzt - letzter_empfang_ms) > EMPFANGS_TIMEOUT);
    // Zaehler-Reset bei Timeout: Controller-Neustart akzeptieren
    if (espnow_timeout) letzter_zaehler = UINT32_MAX;
    digitalWrite(LED_LINK, (espnow_timeout && blink) ? HIGH : LOW);

    // FAULT/NOTAUS: RGB-Anzeige
    // Notaus = orange blinkend (hoechste Prio), Fehler = rot blinkend, OK = aus
    if (notaus_empfangen) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 80, 0) : rgb.Color(0, 0, 0));
    } else if (fehler_flag || espnow_timeout) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 0, 0) : rgb.Color(0, 0, 0));
    } else {
        rgb.clear();
    }
    rgb.show();

    debug_paket_ausgeben(jetzt);
    serial_commands_update();

    // I2C-Ausgabe im Loop-Kontext, niemals im ESP-NOW-Callback.
#if RECEIVER_MODE_TEST_SWEEP
    test_sweep_update();
#elif RECEIVER_MODE_LIVE_I2C
    live_i2c_update();
#endif

    delay(10);
}
