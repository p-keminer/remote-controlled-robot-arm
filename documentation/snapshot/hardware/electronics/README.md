# hardware/electronics

## Zweck

Dieser Ordner dokumentiert die Integration der ESP32-, Sensor- und Debug-Elektronik fuer Sender- und Receiver-Seite.

## Aktueller Stand

Die Dokumentationsbasis fuer `ESP32-S3 DevKitC-1 N16R8` ist angelegt.
Breadboard-, Lochraster-, Pinmapping-, Debug- und Gehaeusekonzept sind beschrieben, aber noch nicht am echten Hardwarebestand validiert.

## Inhalt

- `ESP_INTEGRATION_CONCEPT.md` fuer die Gesamtrolle von Sender- und Receiver-Elektronik
- `PROTO_BREADBOARD_PLAN.md` fuer Bench-Prototyp und fruehe offene Verdrahtung
- `FINAL_PERFBOARD_PLAN.md` fuer den dauerbetriebsnahen Lochraster- und Gehaeusepfad
- `PIN_MAPPING_RESEARCH.md` fuer den ersten boardnahen GPIO-Plan
- `DEBUG_SIGNALS_AND_BUZZER.md` fuer LEDs, RGB-Status und Tonlogik
- `ROADMAP.md` fuer offene Elektronikarbeitspakete

## Regeln

- Breadboard ist nur Bench- und Laborpfad, kein Ziel fuer den koerpergetragenen oder dauerhaften Betrieb.
- Boardnahe Pinbelegungen bleiben vorlaeufig, bis die reale Boardrevision und der Hardwarebestand geprueft sind.
- Externe Statussignale duerfen nicht allein auf die onboard RGB-LED abgestuetzt werden, solange die konkrete Boardrevision nicht verifiziert ist.
- Elektronikdokumente mit externen technischen Annahmen brauchen einen Abschnitt `Recherchequellen`.

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../mechanik/` fuer Traegeorte, Gehaeuse und Kabelentlastung zusammen
- verweist auf `../../preparation/esp32_environment/` fuer Toolchain- und Boardvorbereitung
- spiegelt Kommunikationsannahmen aus `../../COMMUNICATION_FRAMEWORK.md`
- spiegelt Safety- und Security-Annahmen aus `../../SAFETY_FRAMEWORK.md` und `../../SECURITY_FRAMEWORK.md`
