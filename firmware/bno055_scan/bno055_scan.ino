#include <Wire.h>

// I2C-Scanner fuer BNO055 und PCA9548A
// SDA: GPIO8 | SCL: GPIO9 (Projektplan PIN_MAPPING_RESEARCH)
// Validierungsschritt 2 laut Validierungsreihenfolge

#define I2C_SDA 8
#define I2C_SCL 9

// Bekannte Adressen im Projekt
#define ADRESSE_BNO055_STANDARD 0x28  // ADR-Pin auf GND
#define ADRESSE_BNO055_ALT      0x29  // ADR-Pin auf 3.3V
#define ADRESSE_PCA9548A_MIN    0x70  // PCA9548A Adressbereich Anfang
#define ADRESSE_PCA9548A_MAX    0x77  // PCA9548A Adressbereich Ende

void i2c_scan() {
    Serial.println("--- Scan ---");
    uint8_t gefunden = 0;

    for (uint8_t adresse = 1; adresse < 127; adresse++) {
        Wire.beginTransmission(adresse);
        uint8_t fehler = Wire.endTransmission();

        if (fehler == 0) {
            Serial.printf("  0x%02X", adresse);

            if (adresse == ADRESSE_BNO055_STANDARD) Serial.print("  <- BNO055 (ADR=GND)");
            if (adresse == ADRESSE_BNO055_ALT)      Serial.print("  <- BNO055 (ADR=3V3)");
            if (adresse >= ADRESSE_PCA9548A_MIN &&
                adresse <= ADRESSE_PCA9548A_MAX)     Serial.print("  <- PCA9548A Mux");

            Serial.println();
            gefunden++;
        }
    }

    if (gefunden == 0) {
        Serial.println("  Kein Geraet gefunden.");
        Serial.println("  Verdrahtung pruefen: SDA->GPIO8, SCL->GPIO9, 3.3V, GND");
    } else {
        Serial.printf("  %d Geraet(e) gefunden.\n", gefunden);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    Serial.println("=== I2C-Scanner ===");
    Serial.printf("SDA: GPIO%d | SCL: GPIO%d\n", I2C_SDA, I2C_SCL);

    i2c_scan();
}

void loop() {
    delay(5000);
    i2c_scan();
}
