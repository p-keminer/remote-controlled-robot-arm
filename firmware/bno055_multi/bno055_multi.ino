#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// BNO055 Multi-Sensor Test ueber PCA9548A Mux
// I2C: SDA GPIO8 / SCL GPIO9
// Mux-Adresse: 0x70
// Sensor 0: Kanal 0 | Sensor 1: Kanal 1

#define I2C_SDA     8
#define I2C_SCL     9
#define MUX_ADRESSE 0x70
#define BNO_ADRESSE 0x29

#define ANZAHL_SENSOREN 2

Adafruit_BNO055 sensoren[ANZAHL_SENSOREN] = {
    Adafruit_BNO055(55, BNO_ADRESSE, &Wire),
    Adafruit_BNO055(56, BNO_ADRESSE, &Wire),
};

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

bool sensor_initialisieren(uint8_t kanal) {
    mux_kanal_oeffnen(kanal);
    delay(50);
    if (!sensoren[kanal].begin()) {
        Serial.printf("FEHLER: Sensor %d nicht gefunden (Kanal %d)\n", kanal, kanal);
        return false;
    }
    sensoren[kanal].setExtCrystalUse(true);
    Serial.printf("Sensor %d bereit (Kanal %d)\n", kanal, kanal);
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Wire.begin(I2C_SDA, I2C_SCL);

    Serial.println("=== BNO055 Multi-Sensor ===");

    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        sensor_initialisieren(i);
    }

    mux_alle_schliessen();
    Serial.println("Bereit.\n");
}

void loop() {
    for (uint8_t i = 0; i < ANZAHL_SENSOREN; i++) {
        mux_kanal_oeffnen(i);
        delay(5);

        sensors_event_t ereignis;
        sensoren[i].getEvent(&ereignis);

        uint8_t sys_kal, gyro_kal, accel_kal, mag_kal;
        sensoren[i].getCalibration(&sys_kal, &gyro_kal, &accel_kal, &mag_kal);

        Serial.printf("S%d | H:%6.1f R:%6.1f P:%6.1f | Kal Sys:%d Gyro:%d Accel:%d Mag:%d\n",
            i,
            ereignis.orientation.x,
            ereignis.orientation.y,
            ereignis.orientation.z,
            sys_kal, gyro_kal, accel_kal, mag_kal);
    }

    mux_alle_schliessen();
    Serial.println("");
    delay(500);
}
