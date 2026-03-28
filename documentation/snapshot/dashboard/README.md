# dashboard

## Zweck

Dieses Verzeichnis enthaelt die Erweiterungen fuer das bestehende IoT Control Center Dashboard auf dem Pi Zero 2W, die das Live-Monitoring und die Simulation des Roboterarms ermoeglichen.

Die Sensordaten werden per MQTT (Mosquitto) vom Bridge-ESP32 empfangen und im Browser als neue Tabs dargestellt.
Zusaetzlich wird ein MQTT MCP Server bereitgestellt, ueber den Claude direkt auf die Live-Sensordaten zugreifen kann.

Das Dashboard ist ein reines **Entwicklungswerkzeug** und kein Teil des v1-Steuerpfads.

## Aktueller Stand

Die Verzeichnisstruktur, Dokumentation und MQTT-Topic-Struktur sind definiert.
Die Implementierung der Views und des MCP-Servers steht als naechstes an.

## Inhalt

- `DASHBOARD_CONCEPT.md` — Architekturentscheidung, Techstack, MQTT-Topics und Recherchequellen
- `pi_views/` — PHP-Views und JS/CSS fuer die neuen Roboterarm-Tabs (werden auf den Pi deployed)
- `mcp/` — MQTT MCP Server fuer Claude-Zugriff auf Live-Sensordaten

## Regeln

- das Dashboard ist rein beobachtend und darf keine Steuerbefehle senden
- MQTT-Credentials gehoeren in gitignorierte lokale Konfigurationsdateien
- die Paketstruktur muss mit `ImuPaket v4` aus `COMMUNICATION_FRAMEWORK.md` uebereinstimmen
- neue Views muessen dem bestehenden Dark-Theme und der CSP-Policy des IoT Dashboards folgen
- kein `onclick` in HTML — stattdessen `data-click` Attribute (CSP-Kompatibilitaet)

## Schnittstellen/Abhaengigkeiten

- empfaengt Daten per MQTT vom Bridge-ESP32 ueber Mosquitto (`../firmware/esp32_bridge/`)
- integriert sich in das bestehende IoT Control Center auf dem Pi (Nginx, PHP, Chart.js)
- MQTT-Topic-Struktur dokumentiert in `DASHBOARD_CONCEPT.md`
- Paketformat dokumentiert in `../COMMUNICATION_FRAMEWORK.md`
- Sicherheitsregeln in `../SECURITY_FRAMEWORK.md`
