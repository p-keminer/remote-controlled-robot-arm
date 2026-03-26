# firmware/esp32_bridge

## Zweck

Dieser Ordner enthaelt die Firmware fuer den Bridge-ESP32, der als drahtloser Debug-Beobachter fungiert.
Der Bridge-ESP32 empfaengt ImuPaket v3 per ESP-NOW vom Controller und leitet die Daten per WiFi (UDP) an einen Raspberry Pi weiter, auf dem ein Entwicklungs-Dashboard laeuft.

Die Bridge ist ein reines **Entwicklungswerkzeug** und kein Teil des v1-Steuerpfads.
Sie kann keine Befehle zuruecksenden und hat keinen Einfluss auf die Servobewegung.

## Aktueller Stand

Die Verzeichnisstruktur und Dokumentation sind angelegt.
Die Firmware-Implementierung steht als naechstes an.

## Inhalt

- `esp32_bridge.ino` — Hauptfirmware: ESP-NOW Empfang + WiFi UDP-Weiterleitung
- `peer_config.template.h` — Vorlage fuer die Controller-MAC-Adresse
- `wifi_config.template.h` — Vorlage fuer WiFi-Zugangsdaten und Pi-Netzwerkadresse

## Regeln

- die Bridge ist rein empfangend und darf keine Daten in den Steuerpfad einschleusen
- WiFi-Zugangsdaten gehoeren ausschliesslich in die gitignorierte `wifi_config.local.h`
- die MAC-Adresse des Controllers gehoert in die gitignorierte `peer_config.local.h`
- ESP-NOW und WiFi muessen auf dem gleichen Funkkanal laufen
- ein Ausfall der Bridge darf den Controller-Receiver-Pfad nicht beeintraechtigen
- die Bridge validiert empfangene Pakete (Groesse, Pruefsumme, Protokollversion) bevor sie weiterleitet

## Schnittstellen/Abhaengigkeiten

- empfaengt ImuPaket v3 per ESP-NOW vom Controller (`../esp32_controller/`)
- leitet Binaerdaten per UDP an den Pi-Dashboard-Server weiter (`../../dashboard/server/`)
- nutzt die gleiche Paketstruktur wie `../esp32_controller/` und `../esp32_receiver/`
- WiFi-Kanal muss zum ESP-NOW-Kanal des Controllers passen
- Paketformat dokumentiert in `../../COMMUNICATION_FRAMEWORK.md`
