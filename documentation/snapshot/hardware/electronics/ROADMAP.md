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
- [ ] echte Boardrevision der vorhandenen DevKitC-Boards feststellen
- [ ] onboard RGB-LED-Pin am realen Bestand pruefen
- [ ] bevorzugten Pinplan an echter Sender- und Receiver-Hardware bench-validieren
- [ ] BNO055-, Mux- und Flex-Sensor-Pfade gegen den Pinplan verifizieren
- [ ] Verdrahtung und Steckersystem spaeter nach `hardware/verkabelung/` ueberfuehren
- [ ] aus den validierten Bench-Ergebnissen einen realen Schaltplanstand ableiten
