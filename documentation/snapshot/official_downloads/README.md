# official_downloads

## Zweck

Dieser Ordner archiviert offizielle Herstellerdownloads, die als Produkt- und Referenzbasis fuer dieses Projekt relevant sind.
Er trennt den unveraenderten Vendor-Stand bewusst von der eigenen Projektarchitektur, Dokumentation und Firmware.

## Aktueller Stand

Der offizielle Adeept-V4.0-Download fuer den vorhandenen Roboterarm ist in das Repository uebernommen, entpackt und ausgewertet.
Der Bereich dient aktuell als Hersteller-Snapshot fuer Tutorials, Schaltplan, Originalcode und Softwarepakete.

## Inhalt

- `raw/` enthaelt den originalen ZIP-Download des Herstellers
- `extracted/` enthaelt den entpackten Herstellerstand
- `ADEEPT_V4_PACKAGE_REVIEW.md` fasst die fuer dieses Projekt relevanten Erkenntnisse aus dem Download zusammen

## Regeln

- Vendor-Dateien werden hier nicht in Projektlogik umgebaut, sondern nur archiviert und analysiert
- produktbezogene Entscheidungen sollen nach Moeglichkeit auf den offiziellen Snapshot und nicht nur auf Shop-Texte gestuetzt werden
- Abweichungen zwischen Herstellerstand und Projektstand muessen in den zugehoerigen Fachdokumenten sichtbar gemacht werden
- neue Erkenntnisse aus diesem Ordner muessen in den betroffenen Hardware-, Firmware-, Safety- oder Vorbereitungsdokumenten nachgezogen werden
- `raw/` und `extracted/` bleiben externe Archivbereiche und werden nicht in den generierten Dokumentationssnapshot unter `documentation/` gespiegelt
- nur projektgepflegte Markdown-Dokumente wie diese README und `ADEEPT_V4_PACKAGE_REVIEW.md` gelten hier als aktive Quelldokumentation

## Schnittstellen/Abhaengigkeiten

- liefert Herstellergrundlagen fuer `hardware/ADEEPT_ARM_PRODUCT_BASELINE.md`
- liefert Stock-Code- und Boardannahmen fuer `firmware/arduino_arm/README.md`
- liefert Strom- und Aufbauhinweise fuer `hardware/bringup/README.md` und `hardware/electronics/POWER_SUPPLY_CONCEPT.md`
- liefert Setup-Bezug fuer `preparation/arduino_ide_setup/README.md`
