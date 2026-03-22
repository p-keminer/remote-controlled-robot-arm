# Roadmap - hardware/electronics

## Ziel

Eine belastbare Elektronikdokumentation fuer Sender- und Receiver-ESP32 aufbauen, vom offenen Bench-Prototyp bis zum spaeteren dauerbetriebsnahen Aufbau.

## Arbeitspakete

- [x] Elektronikbereich unter `hardware/` einfuehren
- [x] `ESP32-S3 DevKitC-1 N16R8` als aktuelle Boardbasis fuer die Recherche dokumentieren
- [x] Breadboardpfad fuer fruehe Bench-Tests dokumentieren
- [x] Lochraster- und Gehaeusepfad fuer den spaeteren Dauerbetrieb dokumentieren
- [x] erste GPIO-Strategie fuer I2C, ADC, UART, LEDs und Buzzer beschreiben
- [x] Debug- und Warnausgabe ueber LEDs, RGB und Buzzer als Konzept beschreiben
- [x] echte Boardrevision der vorhandenen DevKitC-Boards feststellen
- [x] onboard RGB-LED-Pin am realen Bestand pruefen
- [x] bevorzugten Pinplan fuer I2C und ADC an echter Bench-Hardware validieren
- [x] BNO055-, Mux- und Flex-Sensor-Pfade gegen den Pinplan verifizieren
- [x] aktuellen Bench-Schaltplanstand dokumentieren
- [ ] LED-, Buzzer- und UART-Pfade gegen den bestaetigten Pinplan validieren
- [ ] Verdrahtung und Steckersystem spaeter nach `hardware/verkabelung/` ueberfuehren
- [ ] aus den validierten Bench-Ergebnissen einen dauerbetriebsnahen Schaltplanstand ableiten
