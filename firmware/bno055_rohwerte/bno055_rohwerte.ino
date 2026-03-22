#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// BNO055 Rohwerte-Test
// I2C: SDA GPIO8 / SCL GPIO9
// Adresse: 0x29 (ADR-Pin auf 3.3V)

#define I2C_SDA      8
#define I2C_SCL      9
#define BNO_ADRESSE  0x29
#define MUX_ADRESSE  0x70

Adafruit_BNO055 sensor = Adafruit_BNO055(55, BNO_ADRESSE, &Wire);

void mux_kanal_oeffnen(uint8_t kanal) {
    Wire.beginTransmission(MUX_ADRESSE);
    Wire.write(1 << kanal);
    Wire.endTransmission();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    Serial.println("=== BNO055 Rohwerte ===");

    mux_kanal_oeffnen(0);

    if (!sensor.begin()) {
        Serial.println("FEHLER: BNO055 nicht gefunden.");
        Serial.println("Verdrahtung pruefen: SDA->GPIO8, SCL->GPIO9, 3.3V, GND");
        while (true) { delay(1000); }
    }

    delay(1000);
    sensor.setExtCrystalUse(true);

    uint8_t system_status, self_test, system_fehler;
    sensor.getSystemStatus(&system_status, &self_test, &system_fehler);
    Serial.printf("System-Status: 0x%02X | Self-Test: 0x%02X | Fehler: 0x%02X\n",
        system_status, self_test, system_fehler);

    Serial.println("Bereit. Ausgabe alle 500ms.");
    Serial.println("");
}

void loop() {
    // Euler-Winkel (Heading, Roll, Pitch) in Grad
    sensors_event_t ereignis;
    sensor.getEvent(&ereignis);

    Serial.printf("Euler  | H: %6.1f  R: %6.1f  P: %6.1f\n",
        ereignis.orientation.x,
        ereignis.orientation.y,
        ereignis.orientation.z);

    // Kalibrierungsstatus (je 0-3, 3 = voll kalibriert)
    uint8_t sys_kal, gyro_kal, accel_kal, mag_kal;
    sensor.getCalibration(&sys_kal, &gyro_kal, &accel_kal, &mag_kal);
    Serial.printf("Kalib  | Sys:%d Gyro:%d Accel:%d Mag:%d\n",
        sys_kal, gyro_kal, accel_kal, mag_kal);

    Serial.println("");
    delay(500);
}
