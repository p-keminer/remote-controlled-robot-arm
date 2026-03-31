# dashboard

## Zweck

Dieses Verzeichnis enthaelt die Erweiterungen fuer das bestehende IoT Control Center Dashboard auf dem Pi Zero 2W, die das Live-Monitoring und die Simulation des Roboterarms ermoeglichen.

Die Sensordaten werden per MQTT (Mosquitto) vom Bridge-ESP32 empfangen und im Browser als neue Tabs dargestellt.
Zusaetzlich wird ein MQTT MCP Server bereitgestellt, ueber den Claude direkt auf die Live-Sensordaten zugreifen kann.

Das Dashboard ist ein reines **Entwicklungswerkzeug** und kein Teil des v1-Steuerpfads.

## Aktueller Stand

Die Roboterarm-Erweiterung ist implementiert und auf dem Pi deployed:
- **Robotarm-Tab** mit 4 Sub-Views: Live-Daten, Statistiken, 3D-Modell, Debug-Konsole
- **MQTT-WebSocket** per Nginx Reverse Proxy (wss://host/mqtt → Mosquitto:9001)
- **3D-Simulation** mit realistischem Adeept 5-DOF Arm in Wohnzimmer-Umgebung
- **Wandmontage-Toggle** und Kamera-Reset
- **Kollisionserkennung** (Arm vs. Boden, Waende, Tisch, Wandhalterung)
- **Gelenk-Limits** basierend auf mechanischen Grenzen des Adeept-Arms
- **MCP-Server** fuer Claude-Zugriff auf MQTT-Topics

## Inhalt

- `DASHBOARD_CONCEPT.md` — Architekturentscheidung, Techstack, MQTT-Topics und Recherchequellen
- `ROADMAP.md` — Arbeitspakete und Fortschritt
- `web/` — Frontend-Dateien (PHP-Views, JS, CSS) die auf den Pi deployed werden
  - `web/views/robotarm.php` — PHP-View mit 4 Sub-Tabs (Live, Stats, 3D, Debug)
  - `web/js/robotarm.js` — MQTT-WebSocket-Client, Datenverarbeitung, Charts
  - `web/js/robotarm_3d.js` — Three.js 5-DOF Arm-Modell + Wohnzimmer-Szene
  - `web/css/robotarm.css` — Robotarm-spezifische Styles (ra- Prefix)
- `mcp/` — MQTT MCP Server fuer Claude-Zugriff auf Live-Sensordaten

## Deployment

Die Dateien aus `web/` werden auf den Pi kopiert:
- `web/views/robotarm.php` → `/var/www/html/views/robotarm.php`
- `web/js/robotarm.js` → `/var/www/html/js/robotarm.js`
- `web/js/robotarm_3d.js` → `/var/www/html/js/robotarm_3d.js`
- `web/css/robotarm.css` → `/var/www/html/css/robotarm.css`

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
