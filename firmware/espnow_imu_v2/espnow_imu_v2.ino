#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Controller v2: 3 IMUs + Kalibrierungsanzeige
// I2C: SDA GPIO8 / SCL GPIO9 | Mux: 0x70 | Sensoren: 0x29
// Peer-Adresse kommt aus peer_config.local.h (gitignored)

#include "peer_config.local.h"

#define I2C_SDA           8
#define I2C_SCL           9
#define MUX_ADRESSE       0x70
#define BNO_ADRESSE       0x29
#define ANZAHL_SENSOREN   3
#define SENDE_INTERVALL   50    // ms
#define PROTOKOLL_VERSION 3

#define FLEX_PIN          1
#define FLEX_GERADE       1108
#define FLEX_GEBOGEN      940

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

Adafruit_BNO055 sensoren[ANZAHL_SENSOREN] = {
    Adafruit_BNO055(55, BNO_ADRESSE, &Wire),
    Adafruit_BNO055(56, BNO_ADRESSE, &Wire),
    Adafruit_BNO055(57, BNO_ADRESSE, &Wire),
};

static uint32_t sende_zaehler = 0;

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

void beiBesendung(const wifi_tx_info_t* tx_info, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        Serial.printf("[TX] FEHLER bei #%lu\n", sende_zaehler - 1);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("=== Controller v2: 3 IMUs + Kalib ===");

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(50);
        if (!sensoren[i].begin()) {
            Serial.printf("FEHLER: Sensor %d nicht gefunden\n", i);
        } else {
            sensoren[i].setExtCrystalUse(true);
            Serial.printf("Sensor %d bereit\n", i);
        }
    }
    mux_alle_schliessen();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

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

    analogReadResolution(12);
    Serial.println("Bereit.");
}

float flex_lesen() {
    uint32_t summe = 0;
    for (uint8_t i = 0; i < 16; i++) summe += analogRead(FLEX_PIN);
    uint16_t rohwert = summe / 16;
    float prozent = (float)(FLEX_GERADE - rohwert) / (FLEX_GERADE - FLEX_GEBOGEN) * 100.0f;
    if (prozent < 0.0f)   prozent = 0.0f;
    if (prozent > 100.0f) prozent = 100.0f;
    return prozent;
}

void loop() {
    ImuPaket paket;
    paket.protokoll_version = PROTOKOLL_VERSION;
    paket.zaehler = sende_zaehler++;

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(10);
        sensors_event_t ereignis;
        sensoren[i].getEvent(&ereignis);
        paket.sensoren[i].heading = ereignis.orientation.x;
        paket.sensoren[i].roll    = ereignis.orientation.y;
        paket.sensoren[i].pitch   = ereignis.orientation.z;
        sensoren[i].getCalibration(
            &paket.kalib[i].sys, &paket.kalib[i].gyro,
            &paket.kalib[i].accel, &paket.kalib[i].mag);
    }
    mux_alle_schliessen();

    paket.flex_prozent = flex_lesen();
    paket.pruefsumme = pruefsumme_berechnen(&paket);
    esp_now_send(empfaenger_adresse, (uint8_t*)&paket, sizeof(paket));

    delay(SENDE_INTERVALL);
}
