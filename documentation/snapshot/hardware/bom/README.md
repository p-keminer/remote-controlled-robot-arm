# hardware/bom

## Zweck

Dieser Ordner enthaelt die Stueckliste fuer Elektronik, Mechanikteile und Verbrauchsmaterial.

## Aktueller Stand

Die Struktur ist vorbereitet.
Die eigentliche Stueckliste fuer Prototyp und spaetere Verstetigung wird hier gesammelt.
Die exakte Produktbasis des vorhandenen Adeept-Kits ist in `../ADEEPT_ARM_PRODUCT_BASELINE.md` festgehalten.
Der offizielle Herstellerdownload ist ausgewertet; als Arbeitsstand gelten jetzt `5 aktive Servos + 1 Reserve/Testservo` sowie ein stock-kompatibler `2x18650`-Pfad.
Als aktuelle Beschaffungsbasis fuer Energie und Laden gelten `4x Molicel INR-18650-M35A` ohne Loetfahne plus `1x XTAR VC4SL`.
Reale Bestandsaufnahme, Batteriefach-Passung und Beschaffung bleiben dennoch bis zum echten Unpacking bzw. Kaufabschluss offen.

## Inhalt

- Kernkomponenten des Robotersystems
- Zusatzteile fuer Verkabelung, Montage und Versorgung
- spaetere Unterschiede zwischen Testaufbau und Daueraufbau
- dokumentierte Beschaffungsbasis fuer `2x18650`, Ladepfad und spaetere Stromversorgung
- Abgleich zwischen Produktseite, offizieller Packing List und realem Kit-Inhalt

## Regeln

- Materialien nachvollziehbar benennen und Versionen kenntlich machen
- Ersatz- oder Alternativteile nicht unkommentiert mit Primarkomponenten vermischen
- offizielle Package-List und real vorhandener Bestand muessen sichtbar getrennt bleiben
- Arbeitsannahmen wie `5 aktive Servos + 1 Reserve/Testservo` bleiben markiert, bis der reale Kit-Inhalt dokumentiert ist
- gewaehlte Kaufprodukte bleiben als Projektarbeitsstand markiert, bis reale Passung, Beschaffung und Betrieb bestaetigt sind

## Schnittstellen/Abhaengigkeiten

- wird durch `hardware/README.md` eingeordnet
- kann bei Aufbauaenderungen auf `docs/session_notes/` verweisen

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer die offizielle Package-List mit `0.96'' OLED Display`, `6x AD002 Servo`, `Battery Holder` und `Micro USB Cable`.
- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer den Abgleich zwischen Produktseite, offizieller `Packing List`, Stock-Test und Originalcode.
- [Molicel INR-18650-M35A 3,6 Volt 3500mAh Li-Ion ohne Loetfahne](https://akkuplus.de/molicel-INR-18650-M35A-36-Volt-3500mAh-Li-Ion-ohne-Loetfahne) fuer die aktuell gewaehlte Akkubasis.
- [XTAR VC4SL QC3.0 Ladegeraet](https://www.akkuteile.de/ladegeraete/xtar/xtar-vc4sl-qc3-0-ladegeraet-fuer-li-ion-3-6v-3-7v-und-nimh-1-2v-akkus_500237_2914) fuer das aktuell gewaehlte Ladegeraet.
