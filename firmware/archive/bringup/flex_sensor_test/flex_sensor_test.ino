#include <Arduino.h>

// Flex-Sensor Test
// GPIO1 (ADC1) — Spannungsteiler: 3.3V -> Flex -> GPIO1 -> 10kOhm -> GND
// ADC-Aufloesung: 12 Bit (0-4095), Referenz: 3.3V

#define FLEX_PIN       1
#define ADC_AUFLOESUNG 4096
#define REFERENZ_V     3.3f
#define MESSUNGEN      16    // Mittelwert ueber N Messungen

uint16_t flex_lesen() {
    uint32_t summe = 0;
    for (uint8_t i = 0; i < MESSUNGEN; i++) {
        summe += analogRead(FLEX_PIN);
    }
    return summe / MESSUNGEN;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    analogReadResolution(12);

    Serial.println("=== Flex-Sensor Test ===");
    Serial.println("GPIO1 | ADC1 | 10k Pull-Down");
    Serial.println("Sensor bewegen um Wertebereich zu ermitteln.");
    Serial.println("");
}

void loop() {
    uint16_t rohwert = flex_lesen();
    float spannung   = rohwert * REFERENZ_V / ADC_AUFLOESUNG;

    Serial.printf("Roh: %4d | %.3f V\n", rohwert, spannung);
    delay(200);
}
