# preparation

## Zweck

Dieser Ordner dokumentiert die Vorbereitungsbasis fuer Toolchain, lokale Entwicklungsumgebung und Inbetriebnahme.

## Aktueller Stand

Die Projektvorbereitung ist nicht mehr nur theoretisch, sondern auf den aktiven Prototypstand gezogen:

- Arduino IDE bleibt der praktische Hauptpfad
- das Repo fuehrt verbindlich das Custom-FQBN `esp32:esp32:robotic_arm_s3n16r8`
- Bench- und Bringup-Doku wurden gegen den realen Perfboard-/Prototypstand nachgezogen
- Controller, Bridge, Dashboard und ROS laufen als verifizierter Debug- und Twin-Pfad zusammen

## Inhalt

- `arduino_ide_setup/` fuer den alltaeglichen Build-/Uploadpfad
- `esp32_environment/` fuer Board-, Stack- und Toolchainstand
- `bench_setup/` fuer geordneten Labor- und Prototypbetrieb
- `robot_arm_readiness/` fuer aktuelle Inbetriebnahme- und Freigaberegeln

## Regeln

- Lokale Ist-Staende und sensible Konfigurationsdetails gehoeren in gitignorierte Dateien.
- Historische oder generische Boardpfade werden als Ausnahme markiert, nicht als Standard.
- Vorbereitungsdoku muss mit Firmware-, Hardware- und Security-Doku synchron bleiben.

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../hardware/`, `../firmware/`, `../security/` und `../docs/`
