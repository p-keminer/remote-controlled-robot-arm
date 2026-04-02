# Roadmap - dashboard

## Ziel

Das bestehende IoT Control Center Dashboard um Roboterarm-Views erweitern, die Live-Sensordaten per MQTT empfangen und als Debug-Konsole, Statistiken und 3D-Simulation darstellen. Zusaetzlich einen MQTT MCP Server bereitstellen fuer KI-gestuetztes Live-Debugging.

## Arbeitspakete

### Mosquitto-Konfiguration

- [x] MQTT-User fuer Bridge-ESP32 anlegen (Publish-Rechte auf `robotarm/#`)
- [x] MQTT-User fuer Dashboard-WebSocket anlegen (Subscribe-Rechte auf `robotarm/#`)
- [x] WebSocket-Listener in Mosquitto konfigurieren (Port 9001)
- [x] Nginx Reverse Proxy fuer MQTT-WebSocket auf Port 443 einrichten (`/mqtt` Location)
- [x] CSP-Header um `connect-src wss://` erweitern

### Pi Dashboard Views

- [x] Roboterarm-Tab in `views/dashboard.php` einbinden
- [x] Debug-Konsole: scrollende Tabelle mit Sensordaten und Kalibrierungsstatus
- [x] Statistiken: Paketrate, Fehlerrate, Kalibrierungsgauges (Chart.js)
- [x] 3D-Simulation: Three.js Szene mit realistischem 5-DOF Arm-Modell
- [x] MQTT-Client im Browser (`mqtt.js`) fuer Live-Subscribe auf `robotarm/#`
- [x] Wandmontage-Toggle (90° Rotation, Hoehen-Anpassung)
- [x] Kollisionserkennung (Arm vs. Boden, Waende, Tisch, Wandhalterung)
- [x] Gelenk-Limits basierend auf mechanischen Grenzen
- [x] Wohnzimmer-Umgebung (TV-Wand, Couch, Lampe, Pflanze, Bilder, Teppich)
- [x] Interaktive Objekte auf dem Arbeitstisch (Wuerfel, Kugel, Zylinder, etc.)
- [x] Kamera-Begrenzung innerhalb der Raum-Grenzen

### MQTT MCP Server

- [x] Python MCP Server mit MQTT-Subscribe auf `robotarm/#`
- [x] Tools: `mqtt_subscribe`, `mqtt_read_latest`, `mqtt_list_topics`
- [x] Konfiguration ueber gitignorierte `mqtt_config.local.py`
- [x] Integration in Claude Code als lokaler MCP Server

### OTA-Update (sicherheitskritisch)

- [ ] Tool `ota_flash`: kompiliert lokal, sendet .bin per ArduinoOTA an Bridge-ESP32
- [ ] Tool `ota_status`: prueft ob Bridge erreichbar und OTA-bereit
- [ ] Explizite User-Bestaetigung vor jedem Flash-Vorgang (Pflicht)
- [ ] Konfiguration ueber gitignorierte `ota_config.local.py`
- [ ] OTA-Sicherheitskonzept aus DASHBOARD_CONCEPT.md vollstaendig umsetzen

### Integration

- [x] Nginx WebSocket Proxy verifiziert (HTTP 101 Upgrade)
- [x] Pipeline: Controller → Bridge → MQTT → Mosquitto → WSS → Browser
- [ ] End-to-End Live-Test mit Controller (steht noch aus)
- [ ] Latenz messen: Controller-Send bis Dashboard-Anzeige
- [ ] Pi Zero 2W Performance validieren (CPU, RAM bei 20Hz Datenrate)
