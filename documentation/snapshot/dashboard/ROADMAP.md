# Roadmap - dashboard

## Ziel

Das bestehende IoT Control Center Dashboard um Roboterarm-Views erweitern, die Live-Sensordaten per MQTT empfangen und als Debug-Konsole, Statistiken und 3D-Simulation darstellen. Zusaetzlich einen MQTT MCP Server bereitstellen fuer KI-gestuetztes Live-Debugging.

## Arbeitspakete

### Mosquitto-Konfiguration

- [ ] MQTT-User fuer Bridge-ESP32 anlegen (Publish-Rechte auf `robotarm/#`)
- [ ] MQTT-User fuer MCP-Server anlegen (Subscribe-Rechte auf `robotarm/#`)
- [ ] WebSocket-Listener in Mosquitto konfigurieren
- [ ] Nginx Reverse Proxy fuer MQTT-WebSocket auf Port 443 einrichten
- [ ] CSP-Header um `connect-src wss://` erweitern

### Pi Dashboard Views

- [ ] Roboterarm-Tab in `views/dashboard.php` einbinden
- [ ] Debug-Konsole: scrollende Tabelle mit Sensordaten und Kalibrierungsstatus
- [ ] Statistiken: Paketrate, Fehlerrate, Kalibrierungsgauges (Chart.js)
- [ ] 3D-Simulation: Three.js Szene mit vereinfachtem 5-DOF Arm-Modell
- [ ] MQTT-Client im Browser (`mqtt.js`) fuer Live-Subscribe auf `robotarm/#`

### MQTT MCP Server

- [ ] Python MCP Server mit MQTT-Subscribe auf `robotarm/#`
- [ ] Tools: `mqtt_subscribe`, `mqtt_read_latest`, `mqtt_list_topics`
- [ ] Konfiguration ueber gitignorierte `mqtt_config.local.py`
- [ ] Integration in Claude Code als lokaler MCP Server

### OTA-Update (sicherheitskritisch)

- [ ] Tool `ota_flash`: kompiliert lokal, sendet .bin per ArduinoOTA an Bridge-ESP32
- [ ] Tool `ota_status`: prueft ob Bridge erreichbar und OTA-bereit
- [ ] Explizite User-Bestaetigung vor jedem Flash-Vorgang (Pflicht)
- [ ] Konfiguration ueber gitignorierte `ota_config.local.py`
- [ ] OTA-Sicherheitskonzept aus DASHBOARD_CONCEPT.md vollstaendig umsetzen

### Integration

- [ ] Vollintegration: Bridge → Mosquitto → Dashboard + MCP gleichzeitig testen
- [ ] Latenz messen: Controller-Send bis Dashboard-Anzeige
- [ ] Pi Zero 2W Performance validieren (CPU, RAM bei 20Hz Datenrate)
