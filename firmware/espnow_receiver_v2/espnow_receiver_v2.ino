#include <WiFi.h>
#include <esp_now.h>

// ARCHIV — historischer Bench-Stand v2 (abgeloest 2026-03-26)
// Aktuelle Version: siehe firmware/esp32_receiver/
// Receiver v2: 3 IMUs + Kalibrierungsanzeige
// Prueft: Protokollversion, Pruefsumme, Frame-Frische

#define PROTOKOLL_VERSION 3
#define ANZAHL_SENSOREN   3

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

static uint32_t letzter_zaehler = UINT32_MAX;

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
        Serial.printf("[RX] Unerwartete Paketgroesse: %d\n", laenge);
        return;
    }

    ImuPaket paket;
    memcpy(&paket, daten, sizeof(paket));

    if (paket.protokoll_version != PROTOKOLL_VERSION) {
        Serial.printf("[RX] Unbekannte Protokollversion: %d\n", paket.protokoll_version);
        return;
    }

    if (paket.pruefsumme != pruefsumme_berechnen(&paket)) {
        Serial.printf("[RX] Pruefsumme ungueltig: #%lu\n", paket.zaehler);
        return;
    }

    if (paket.zaehler <= letzter_zaehler && letzter_zaehler != UINT32_MAX) {
        Serial.printf("[RX] Veraltetes Frame verworfen: #%lu\n", paket.zaehler);
        return;
    }
    letzter_zaehler = paket.zaehler;

    Serial.printf("#%lu\n", paket.zaehler);
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        Serial.printf("  S%d | H:%6.1f  R:%6.1f  P:%6.1f  [S%d G%d A%d M%d]\n", i,
            paket.sensoren[i].heading,
            paket.sensoren[i].roll,
            paket.sensoren[i].pitch,
            paket.kalib[i].sys, paket.kalib[i].gyro,
            paket.kalib[i].accel, paket.kalib[i].mag);
    }
    Serial.printf("  Flex: %5.1f%%\n", paket.flex_prozent);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("=== Receiver v2: 3 IMUs + Kalib ===");
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    if (esp_now_init() != ESP_OK) {
        Serial.println("FEHLER: ESP-NOW init");
        return;
    }

    esp_now_register_recv_cb(beiEmpfang);
    Serial.println("Bereit.");
}

void loop() {
    delay(1000);
}
