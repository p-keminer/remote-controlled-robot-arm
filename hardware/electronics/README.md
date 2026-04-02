# hardware/electronics

## Zweck

Dieser Ordner dokumentiert die Integration der ESP32-, Sensor- und Debug-Elektronik fuer Sender- und Receiver-Seite.

## Aktueller Stand

Die Dokumentationsbasis fuer `ESP32-S3 DevKitC-1 N16R8` ist angelegt und teilweise real validiert.
Boardrevision, RGB-Pin, I2C-Pinplan, ADC-Pfad und der aktuelle Bench-Schaltplan sind bestaetigt.
Zusaetzlich ist jetzt ein eigenes Stromversorgungskonzept verfuegbar, das den offiziellen Adeept-Stock-Pfad vom spaeteren Projektbetrieb trennt.
Die I2C-Grundkette (Receiver GPIO13/14 → Arduino A4/A5, Slave 0x42) ist bench-validiert (bestaetigt 2026-04-02). LED-, Lochraster- und Gehaeusethemen sind konzeptionell dokumentiert, in der praktischen Umsetzung aber noch offen. Buzzer wurde als Projektentscheidung gestrichen — LEDs und RGB reichen fuer Debugging.

## Inhalt

- `ESP_INTEGRATION_CONCEPT.md` fuer die Gesamtrolle von Sender- und Receiver-Elektronik
- `../ADEEPT_ARM_PRODUCT_BASELINE.md` fuer Produktbasis, Package-List und Stromversorgungsrahmen des Originalarms
- `POWER_SUPPLY_CONCEPT.md` fuer die Stromversorgungsstrategie von Stock-Test, Bench und Dauerbetrieb
- `PROTO_BREADBOARD_PLAN.md` fuer Bench-Prototyp und fruehe offene Verdrahtung
- `FINAL_PERFBOARD_PLAN.md` fuer den dauerbetriebsnahen Lochraster- und Gehaeusepfad
- `PIN_MAPPING_RESEARCH.md` fuer den ersten boardnahen GPIO-Plan
- `PIN_MAPPING_CONFIRMED.md` fuer real bestaetigte GPIO-Belegungen
- `SCHEMATIC_CURRENT.md` fuer den aktuellen Bench-Schaltplanstand
- `DEBUG_SIGNALS_AND_BUZZER.md` fuer LEDs, RGB-Status und Tonlogik
- `ROADMAP.md` fuer offene Elektronikarbeitspakete

## Regeln

- Breadboard ist nur Bench- und Laborpfad, kein Ziel fuer den koerpergetragenen oder dauerhaften Betrieb.
- Nur in `PIN_MAPPING_CONFIRMED.md` und `SCHEMATIC_CURRENT.md` dokumentierte Werte gelten als real bestaetigt.
- Externe Statussignale duerfen nicht allein auf die onboard RGB-LED abgestuetzt werden; sie bleibt ein Zusatzsignal.
- Elektronikdokumente mit externen technischen Annahmen brauchen einen Abschnitt `Recherchequellen`.

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../mechanik/` fuer Traegeorte, Gehaeuse und Kabelentlastung zusammen
- verweist auf `../../preparation/esp32_environment/` fuer Toolchain- und Boardvorbereitung
- verweist auf `../../official_downloads/` fuer offiziellen Stock-Code und Herstellerunterlagen
- spiegelt Kommunikationsannahmen aus `../../COMMUNICATION_FRAMEWORK.md`
- spiegelt Safety- und Security-Annahmen aus `../../SAFETY_FRAMEWORK.md` und `../../SECURITY_FRAMEWORK.md`
