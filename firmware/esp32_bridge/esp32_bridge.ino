#include <WiFi.h>
#include <esp_now.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>

// Bridge-ESP32: ESP-NOW Empfang → MQTT Weiterleitung an Pi
// Reines Entwicklungswerkzeug — kein Teil des v1-Steuerpfads.
// Die Bridge empfaengt nur, sie sendet keine Daten in den Steuerpfad.
//
// Datenpfad:
//   Controller ESP32 ──ESP-NOW──> Bridge ESP32 ──WiFi/MQTT──> Mosquitto (Pi)
//
// Konfiguration aus gitignorierten Dateien:
//   peer_config.local.h  — Controller-MAC (Absendervalidierung)
//   wifi_config.local.h  — WiFi, MQTT-Broker, OTA-Passwort
//
// LED-Debugging (invertiert: aus = OK, blinken = Problem):
//   GPIO4  Gruen  — blinkt wenn WiFi getrennt
//   GPIO5  Blau   — blinkt wenn ESP-NOW Timeout
//   GPIO7  Weiss  — blinkt wenn MQTT getrennt
//   GPIO48 RGB    — rot blinkend bei FAULT (allgemeiner Fehler)

#include "peer_config.local.h"
#include "wifi_config.local.h"

// ============================================================
// Paketstruktur — identisch mit Controller und Receiver
// ============================================================

#define ANZAHL_SENSOREN   3
#define PROTOKOLL_VERSION 3

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
    uint8_t     protokoll_version;
    uint8_t     pruefsumme;
} ImuPaket;

// ============================================================
// LED-Pins
// ============================================================

#define LED_WIFI    4   // Gruen  — blinkt wenn WiFi getrennt
#define LED_LINK    5   // Blau   — blinkt wenn ESP-NOW Timeout
#define LED_MQTT    7   // Weiss  — blinkt wenn MQTT getrennt
#define RGB_PIN    48   // Interne RGB-LED
#define RGB_ANZAHL  1

Adafruit_NeoPixel rgb(RGB_ANZAHL, RGB_PIN, NEO_GRB + NEO_KHZ800);

// ============================================================
// Globale Variablen
// ============================================================

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);

static volatile bool neues_paket = false;
static volatile ImuPaket empfangenes_paket;
static uint32_t letzter_zaehler = UINT32_MAX;
static unsigned long letzter_empfang_ms = 0;
static unsigned long boot_zeit_ms = 0;

// Statistik
static uint32_t pakete_gesamt = 0;
static uint32_t fehler_gesamt = 0;
static unsigned long letzte_status_ms = 0;
static float pps = 0.0f;
static uint32_t pps_zaehler = 0;
static unsigned long pps_start_ms = 0;

// OTA
static bool ota_aktiv = false;

// ============================================================
// Pruefsumme — identisch mit Controller
// ============================================================

uint8_t pruefsumme_berechnen(const ImuPaket* paket) {
    const uint8_t* bytes = (const uint8_t*)paket;
    uint8_t summe = 0;
    for (size_t i = 0; i < sizeof(ImuPaket) - 1; i++) {
        summe ^= bytes[i];
    }
    return summe;
}

// ============================================================
// ESP-NOW Empfangs-Callback
// ============================================================

void beiEmpfang(const esp_now_recv_info_t* info, const uint8_t* daten, int laenge) {
    if (laenge != sizeof(ImuPaket)) {
        fehler_gesamt++;
        return;
    }

    if (memcmp(info->src_addr, controller_adresse, 6) != 0) {
        fehler_gesamt++;
        return;
    }

    const ImuPaket* paket = (const ImuPaket*)daten;

    if (paket->protokoll_version != PROTOKOLL_VERSION) {
        fehler_gesamt++;
        return;
    }

    if (paket->pruefsumme != pruefsumme_berechnen(paket)) {
        fehler_gesamt++;
        return;
    }

    memcpy((void*)&empfangenes_paket, daten, sizeof(ImuPaket));
    neues_paket = true;
    letzter_empfang_ms = millis();
    pakete_gesamt++;
    pps_zaehler++;
}

// ============================================================
// MQTT: Paket als kompaktes JSON publishen
// ============================================================

void mqtt_imu_senden(const ImuPaket* p) {
    char json[384];
    int n = snprintf(json, sizeof(json),
        "{\"z\":%lu,"
        "\"s\":["
          "{\"h\":%.1f,\"r\":%.1f,\"p\":%.1f},"
          "{\"h\":%.1f,\"r\":%.1f,\"p\":%.1f},"
          "{\"h\":%.1f,\"r\":%.1f,\"p\":%.1f}],"
        "\"k\":["
          "{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d},"
          "{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d},"
          "{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d}],"
        "\"f\":%.1f,\"v\":%d}",
        (unsigned long)p->zaehler,
        p->sensoren[0].heading, p->sensoren[0].roll, p->sensoren[0].pitch,
        p->sensoren[1].heading, p->sensoren[1].roll, p->sensoren[1].pitch,
        p->sensoren[2].heading, p->sensoren[2].roll, p->sensoren[2].pitch,
        p->kalib[0].sys, p->kalib[0].gyro, p->kalib[0].accel, p->kalib[0].mag,
        p->kalib[1].sys, p->kalib[1].gyro, p->kalib[1].accel, p->kalib[1].mag,
        p->kalib[2].sys, p->kalib[2].gyro, p->kalib[2].accel, p->kalib[2].mag,
        p->flex_prozent, p->protokoll_version);

    if (n > 0 && n < (int)sizeof(json)) {
        mqtt.publish(TOPIC_IMU, json);
    }
}

void mqtt_status_senden() {
    unsigned long jetzt = millis();
    unsigned long delta = jetzt - pps_start_ms;
    if (delta >= 1000) {
        pps = (float)pps_zaehler * 1000.0f / delta;
        pps_zaehler = 0;
        pps_start_ms = jetzt;
    }

    float fehlerrate = (pakete_gesamt + fehler_gesamt > 0)
        ? (float)fehler_gesamt / (pakete_gesamt + fehler_gesamt)
        : 0.0f;

    char json[192];
    snprintf(json, sizeof(json),
        "{\"wifi_rssi\":%d,"
        "\"uptime_s\":%lu,"
        "\"pakete_gesamt\":%lu,"
        "\"fehler_gesamt\":%lu,"
        "\"fehlerrate\":%.4f,"
        "\"pps\":%.1f}",
        WiFi.RSSI(),
        (unsigned long)((jetzt - boot_zeit_ms) / 1000),
        (unsigned long)pakete_gesamt,
        (unsigned long)fehler_gesamt,
        fehlerrate,
        pps);

    mqtt.publish(TOPIC_STATUS, json, true);
}

// ============================================================
// MQTT: Kalibrierungsstatus separat (bei Aenderung)
// ============================================================

static KalibStatus letzte_kalib[ANZAHL_SENSOREN] = {};

void mqtt_kalib_pruefen(const ImuPaket* p) {
    bool geaendert = false;
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        if (memcmp(&p->kalib[i], &letzte_kalib[i], sizeof(KalibStatus)) != 0) {
            geaendert = true;
            memcpy(&letzte_kalib[i], &p->kalib[i], sizeof(KalibStatus));
        }
    }

    if (!geaendert) return;

    char json[192];
    snprintf(json, sizeof(json),
        "[{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d},"
         "{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d},"
         "{\"s\":%d,\"g\":%d,\"a\":%d,\"m\":%d}]",
        p->kalib[0].sys, p->kalib[0].gyro, p->kalib[0].accel, p->kalib[0].mag,
        p->kalib[1].sys, p->kalib[1].gyro, p->kalib[1].accel, p->kalib[1].mag,
        p->kalib[2].sys, p->kalib[2].gyro, p->kalib[2].accel, p->kalib[2].mag);

    mqtt.publish(TOPIC_KALIB, json, true);
}

// ============================================================
// WiFi-Verbindung
// ============================================================

void wifi_verbinden() {
    if (WiFi.status() == WL_CONNECTED) return;

    Serial.printf("[WIFI] Verbinde mit %s...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORT);

    uint8_t versuche = 0;
    while (WiFi.status() != WL_CONNECTED && versuche < 40) {
        delay(250);
        versuche++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WIFI] Verbunden! IP: %s  Kanal: %d\n",
            WiFi.localIP().toString().c_str(), WiFi.channel());
    } else {
        Serial.println("[WIFI] FEHLER: Verbindung fehlgeschlagen");
    }
}

// ============================================================
// MQTT-Verbindung
// ============================================================

void mqtt_verbinden() {
    if (mqtt.connected()) return;

    Serial.printf("[MQTT] Verbinde mit %s:%d...\n", MQTT_BROKER, MQTT_PORT);
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setBufferSize(512);

    if (mqtt.connect(OTA_HOSTNAME, MQTT_USER, MQTT_PASSWORT)) {
        Serial.println("[MQTT] Verbunden!");
    } else {
        Serial.printf("[MQTT] FEHLER: rc=%d\n", mqtt.state());
    }
}

// ============================================================
// OTA Setup (passwortgeschuetzt)
// ============================================================

void ota_setup() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORT);

    ArduinoOTA.onStart([]() {
        ota_aktiv = true;
        Serial.println("[OTA] Update gestartet...");
        if (mqtt.connected()) {
            char log_msg[128];
            snprintf(log_msg, sizeof(log_msg),
                "{\"event\":\"start\",\"ip\":\"%s\",\"ts\":%lu}",
                WiFi.localIP().toString().c_str(),
                (unsigned long)(millis() / 1000));
            mqtt.publish(TOPIC_OTA_LOG, log_msg);
            mqtt.loop();
        }
    });

    ArduinoOTA.onEnd([]() {
        ota_aktiv = false;
        Serial.println("[OTA] Update abgeschlossen!");
        if (mqtt.connected()) {
            mqtt.publish(TOPIC_OTA_LOG, "{\"event\":\"success\"}");
            mqtt.loop();
        }
    });

    ArduinoOTA.onError([](ota_error_t error) {
        ota_aktiv = false;
        Serial.printf("[OTA] FEHLER: %d\n", error);
        if (mqtt.connected()) {
            char log_msg[64];
            snprintf(log_msg, sizeof(log_msg),
                "{\"event\":\"error\",\"code\":%d}", error);
            mqtt.publish(TOPIC_OTA_LOG, log_msg);
            mqtt.loop();
        }
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static uint8_t letzter_prozent = 0;
        uint8_t prozent = (progress / (total / 100));
        if (prozent != letzter_prozent && prozent % 25 == 0) {
            Serial.printf("[OTA] %d%%\n", prozent);
            letzter_prozent = prozent;
        }
    });

    ArduinoOTA.begin();
    Serial.printf("[OTA] Bereit (Host: %s)\n", OTA_HOSTNAME);
}

// ============================================================
// LEDs
// ============================================================

void leds_init() {
    pinMode(LED_LINK, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    pinMode(LED_MQTT, OUTPUT);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_MQTT, LOW);

    rgb.begin();
    rgb.setBrightness(30);
    rgb.clear();
    rgb.show();

    // Starttest: alle LEDs kurz an
    digitalWrite(LED_LINK, HIGH);
    digitalWrite(LED_WIFI, HIGH);
    digitalWrite(LED_MQTT, HIGH);
    rgb.setPixelColor(0, rgb.Color(255, 0, 0));
    rgb.show();
    delay(300);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_MQTT, LOW);
    rgb.clear();
    rgb.show();
}

void leds_aktualisieren() {
    unsigned long jetzt = millis();
    bool blink = (jetzt / 500) % 2;  // 1Hz Blinktakt

    // WiFi getrennt: Gruen blinkt
    bool wifi_problem = (WiFi.status() != WL_CONNECTED);
    digitalWrite(LED_WIFI, (wifi_problem && blink) ? HIGH : LOW);

    // ESP-NOW Timeout: Blau blinkt
    bool espnow_timeout = false;
    if (letzter_empfang_ms > 0 && (jetzt - letzter_empfang_ms > 2000)) espnow_timeout = true;
    if (letzter_empfang_ms == 0 && jetzt > 10000) espnow_timeout = true;
    digitalWrite(LED_LINK, (espnow_timeout && blink) ? HIGH : LOW);

    // MQTT getrennt: Weiss blinkt
    bool mqtt_problem = !mqtt.connected();
    digitalWrite(LED_MQTT, (mqtt_problem && blink) ? HIGH : LOW);

    // RGB: rot blinkend wenn irgendein Problem
    bool fault = wifi_problem || espnow_timeout || mqtt_problem;
    if (fault) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 0, 0) : rgb.Color(0, 0, 0));
    } else {
        rgb.clear();
    }
    rgb.show();
}

// ============================================================
// Setup
// ============================================================

void setup() {
    Serial.begin(115200);
    delay(1000);

    boot_zeit_ms = millis();
    leds_init();

    Serial.println("=== Bridge-ESP32: ESP-NOW -> MQTT ===");
    Serial.println("[INFO] Reines Entwicklungswerkzeug — kein Steuerpfad");

    WiFi.mode(WIFI_STA);
    Serial.printf("[INFO] MAC: %s\n", WiFi.macAddress().c_str());

    wifi_verbinden();

    if (esp_now_init() != ESP_OK) {
        Serial.println("[FEHLER] ESP-NOW init fehlgeschlagen");
        return;
    }
    esp_now_register_recv_cb(beiEmpfang);
    Serial.println("[ESP-NOW] Empfangs-Callback registriert");

    mqtt_verbinden();
    ota_setup();

    pps_start_ms = millis();
    Serial.println("[INFO] Bereit. LEDs aus = alles OK.");
}

// ============================================================
// Loop
// ============================================================

void loop() {
    ArduinoOTA.handle();
    if (ota_aktiv) return;

    if (WiFi.status() != WL_CONNECTED) {
        wifi_verbinden();
    }

    if (WiFi.status() == WL_CONNECTED) {
        if (!mqtt.connected()) {
            mqtt_verbinden();
        }
        mqtt.loop();
    }

    if (neues_paket) {
        neues_paket = false;

        ImuPaket lokale_kopie;
        memcpy(&lokale_kopie, (void*)&empfangenes_paket, sizeof(ImuPaket));

        if (mqtt.connected()) {
            mqtt_imu_senden(&lokale_kopie);
            mqtt_kalib_pruefen(&lokale_kopie);
        }
    }

    unsigned long jetzt = millis();
    if (jetzt - letzte_status_ms >= 1000) {
        letzte_status_ms = jetzt;
        if (mqtt.connected()) {
            mqtt_status_senden();
        }
    }

    leds_aktualisieren();
}
