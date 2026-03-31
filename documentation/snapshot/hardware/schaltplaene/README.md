# hardware/schaltplaene

## Zweck

Dieser Ordner ist fuer elektrische Schaltplaene, Blockdiagramme und Schaltungsvarianten reserviert.

## Aktueller Stand

Der Ordner ist vorbereitet, konkrete Schaltunterlagen folgen spaeter.
Vor modifizierten Schaltunterlagen soll zuerst die originale Adeept-Board- und Servoversorgung gegen den Stock-Aufbau dokumentiert werden.
Der importierte Herstellerstand mit `Adeept_Schematic_for_ArmBoard_V3.0.pdf` ist bereits unter `official_downloads/` archiviert und bildet die offizielle Ausgangsbasis fuer spaetere Projektschaltplaene.

## Inhalt

- Schaltplaene fuer Controller-Seite
- Schaltplaene fuer Receiver- und Arduino-Seite
- Versionshinweise zu relevanten Elektrikaenderungen

## Regeln

- jede Schaltungsvariante klar datieren oder benennen
- Unterschiede zwischen Prototyp und spaeterem Daueraufbau kenntlich machen

## Schnittstellen/Abhaengigkeiten

- wird durch `hardware/README.md` eingeordnet
- kann mit `hardware/verkabelung/` und `docs/photos/` querreferenziert werden
- verweist fuer die offizielle Ausgangsbasis auf `../ADEEPT_ARM_PRODUCT_BASELINE.md` und `../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md`
- spaetere Projektschaltplaene muessen den Herstellerbezug `../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Adeept_Schematic_for_ArmBoard_V3.0.pdf` sauber vom eigenen Umbaupfad trennen
