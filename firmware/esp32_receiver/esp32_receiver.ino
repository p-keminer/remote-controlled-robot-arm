#include <WiFi.h>
#include <esp_now.h>

// Receiver: IMU-Pakete empfangen und validieren
// Prueft: Protokollversion, Pruefsumme, Frame-Frische
//
// LED-Debugging (alle mit 100 Ohm Vorwiderstand):
//   GPIO4  Gruen  — LINK (ESP-NOW Empfang aktiv)
//   GPIO5  Blau   — UART (Weiterleitung aktiv, spaeter)
//   GPIO6  Gelb   — FAULT (Fehler: Pruefsumme, Version, Timeout)

#define PROTOKOLL_VERSION 3
#define ANZAHL_SENSOREN   3

#define LED_LINK          4   // Gruen — Empfang laeuft
#define LED_UART          5   // Blau  — UART-Weiterleitung (spaeter)
#define LED_FAULT         6   // Gelb  — Fehler

#define EMPFANGS_TIMEOUT  2000  // ms ohne Paket = Verbindung verloren

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
static unsigned long letzter_empfang_ms = 1;  // >0 damit Timeout ab Boot greift
static bool fehler_flag = false;

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
        fehler_flag = true;
        return;
    }

    ImuPaket paket;
    memcpy(&paket, daten, sizeof(paket));

    if (paket.protokoll_version != PROTOKOLL_VERSION) {
        Serial.printf("[RX] Unbekannte Protokollversion: %d\n", paket.protokoll_version);
        fehler_flag = true;
        return;
    }

    if (paket.pruefsumme != pruefsumme_berechnen(&paket)) {
        Serial.printf("[RX] Pruefsumme ungueltig: #%lu\n", paket.zaehler);
        fehler_flag = true;
        return;
    }

    if (paket.zaehler <= letzter_zaehler && letzter_zaehler != UINT32_MAX) {
        Serial.printf("[RX] Veraltetes Frame verworfen: #%lu\n", paket.zaehler);
        return;
    }
    letzter_zaehler = paket.zaehler;
    letzter_empfang_ms = millis();
    fehler_flag = false;

    // LINK-LED an bei erfolgreichem Empfang
    digitalWrite(LED_LINK, HIGH);

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

void leds_init() {
    pinMode(LED_LINK, OUTPUT);
    pinMode(LED_UART, OUTPUT);
    pinMode(LED_FAULT, OUTPUT);

    // Starttest: alle LEDs kurz an
    digitalWrite(LED_LINK, HIGH);
    digitalWrite(LED_UART, HIGH);
    digitalWrite(LED_FAULT, HIGH);
    delay(300);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_UART, LOW);
    digitalWrite(LED_FAULT, LOW);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    leds_init();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("=== Receiver: IMU-Daten + LEDs ===");
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    if (esp_now_init() != ESP_OK) {
        Serial.println("FEHLER: ESP-NOW init");
        return;
    }

    esp_now_register_recv_cb(beiEmpfang);
    Serial.println("Bereit.");
}

void loop() {
    unsigned long jetzt = millis();

    // Timeout: kein Paket seit EMPFANGS_TIMEOUT ms
    if (letzter_empfang_ms > 0 && (jetzt - letzter_empfang_ms) > EMPFANGS_TIMEOUT) {
        digitalWrite(LED_LINK, LOW);
        digitalWrite(LED_FAULT, HIGH);
    }

    // Fehler-LED bei Validierungsfehler
    if (fehler_flag) {
        digitalWrite(LED_FAULT, HIGH);
    } else if (letzter_empfang_ms > 0 && (jetzt - letzter_empfang_ms) <= EMPFANGS_TIMEOUT) {
        digitalWrite(LED_FAULT, LOW);
    }

    // UART-LED bleibt aus bis UART implementiert ist
    // digitalWrite(LED_UART, HIGH);

    delay(100);
}
