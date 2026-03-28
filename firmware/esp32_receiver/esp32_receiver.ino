#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Adafruit_NeoPixel.h>

// Receiver: IMU-Pakete empfangen und validieren
// Prueft: Protokollversion, Pruefsumme, Frame-Frische
//
// LED-Debugging (invertiert: aus = OK, blinken = Problem):
//   GPIO4  Gruen  — blinkt wenn UART-Problem (spaeter)
//   GPIO5  Blau   — blinkt wenn ESP-NOW Timeout
//   GPIO48 RGB    — rot blinkend bei FAULT (Pruefsumme, Version)

#define PROTOKOLL_VERSION 3
#define ANZAHL_SENSOREN   3

#define LED_UART          4   // Gruen — UART-Problem
#define LED_LINK          5   // Blau  — ESP-NOW Timeout
#define RGB_PIN          48   // Interne RGB-LED
#define RGB_ANZAHL        1

#define EMPFANGS_TIMEOUT  2000  // ms ohne Paket = Verbindung verloren

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
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_UART, LOW);

    rgb.begin();
    rgb.setBrightness(30);
    rgb.clear();
    rgb.show();

    // Starttest: alle LEDs kurz an
    digitalWrite(LED_LINK, HIGH);
    digitalWrite(LED_UART, HIGH);
    rgb.setPixelColor(0, rgb.Color(255, 0, 0));
    rgb.show();
    delay(300);
    digitalWrite(LED_LINK, LOW);
    digitalWrite(LED_UART, LOW);
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
    Serial.println("Bereit. LEDs aus = alles OK.");
}

void loop() {
    unsigned long jetzt = millis();
    bool blink = (jetzt / 500) % 2;  // 1Hz Blinktakt

    // ESP-NOW Timeout: Blau blinkt
    bool espnow_timeout = (letzter_empfang_ms > 0 && (jetzt - letzter_empfang_ms) > EMPFANGS_TIMEOUT);
    digitalWrite(LED_LINK, (espnow_timeout && blink) ? HIGH : LOW);

    // FAULT: RGB rot blinkend bei Validierungsfehler oder Timeout
    bool fault = fehler_flag || espnow_timeout;
    if (fault) {
        rgb.setPixelColor(0, blink ? rgb.Color(255, 0, 0) : rgb.Color(0, 0, 0));
    } else {
        rgb.clear();
    }
    rgb.show();

    delay(100);
}
