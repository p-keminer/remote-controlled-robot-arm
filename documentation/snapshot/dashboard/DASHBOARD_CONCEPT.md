# Dashboard Concept

Dieses Dokument beschreibt die Architekturentscheidung, den Techstack und die Abgrenzung des Roboterarm-Entwicklungs-Dashboards.

## Einordnung

Das Dashboard ist ein **Entwicklungswerkzeug** (Bench-/Debug-Tool) und kein Teil des v1-Steuerpfads.
Es dient dazu, Sensordaten live zu beobachten, die Armkonfiguration visuell zu pruefen und Kommunikationsstatistiken zu ueberwachen.
WiFi wird ausschliesslich zwischen dem Bridge-ESP32 und dem Raspberry Pi genutzt — der v1-Steuerpfad (ESP-NOW → UART → Arduino) bleibt davon unberuehrt.

Das Dashboard wird als **Erweiterung des bestehenden IoT Control Center Dashboards** auf dem Pi Zero 2W realisiert.
Es wird kein separater Server aufgesetzt. Stattdessen werden neue Views und ein MQTT-basierter Datenpfad in die bestehende Infrastruktur integriert.

## Datenpfad

```
Controller ESP32 ──ESP-NOW──> Receiver ESP32 ──UART──> Arduino (unveraendert)
       │
       └──ESP-NOW (2. Peer)──> Bridge ESP32 ──WiFi──> Mosquitto (MQTT Broker, Port 1883)
                                                              │
                                                    Topic: robotarm/imu
                                                    Topic: robotarm/status
                                                              │
                                              ┌───────────────┼───────────────┐
                                              │               │               │
                                    Dashboard View    MQTT MCP Server    api.php
                                    (MQTT over WS)    (Claude Debug)    (Datenablage)
```

## Bestehende Infrastruktur auf dem Pi

Das IoT Control Center Dashboard laeuft bereits mit folgender Architektur:

- **Nginx** auf Port 443 (HTTPS, Self-Signed) — Dashboard mit Session-Auth, CSP, HSTS
- **Nginx** auf Port 81 — API-Endpunkt fuer ESPs (`api.php`, Token-geschuetzt)
- **PHP 8.4-FPM** — Backend-Logik, Session-Management, CSRF-Schutz
- **Mosquitto MQTT Broker** auf Port 1883 — Passwort-geschuetzt, kein anonymer Zugriff
- **Chart.js** — Telemetrie-Visualisierung
- **MJPG-Streamer** — Kamera-Stream
- **alarm_monitor.py** — Serial-Listener mit Kamera-Aufnahme

Das Dashboard hat ein Tab-basiertes Layout (Dashboard, Logs, Diagnose, Settings, Audit).
Neue Roboterarm-Views werden als zusaetzliche Tabs eingebunden.

## Techstack-Entscheidung

### Bridge → Pi: MQTT (statt UDP)

- **Mosquitto laeuft bereits** auf dem Pi — kein neuer Port, kein neuer Service
- **Passwort-Authentifizierung** — kein offener UDP-Listener noetig
- **Topic-basiert** — saubere Trennung: `robotarm/imu` fuer Sensordaten, `robotarm/status` fuer Bridge-Status
- **Retained Messages** — letzter Wert bleibt verfuegbar fuer spaet verbindende Clients
- **QoS 0** (fire-and-forget) — minimaler Overhead, vergleichbar mit UDP
- **Mehrere Subscriber** — Dashboard, MQTT MCP Server und api.php koennen gleichzeitig mitlesen

### Pi → Browser: MQTT over WebSocket

- Mosquitto unterstuetzt WebSocket-Listener (konfigurierbar)
- Nginx kann WebSocket per Reverse Proxy auf Port 443 durchleiten
- Browser nutzt `mqtt.js` (MQTT-Client-Bibliothek) fuer direkten Subscribe
- kein separater WebSocket-Server noetig — Mosquitto uebernimmt das

### Frontend: Erweiterung des bestehenden Dashboards

- Neue Tabs im bestehenden `views/dashboard.php`
- Gleicher Dark-Theme-Style (`css/style.css`)
- Gleiche CSP-Kompatibilitaet (data-click statt onclick)
- Three.js fuer die 3D-Arm-Simulation (per CDN, in CSP erlaubt)
- Chart.js bereits vorhanden fuer Statistik-Graphen

### MQTT MCP Server (Claude Debug-Zugang)

- Separater MCP-Server der sich per MQTT an Mosquitto verbindet
- Claude kann direkt Topics subscriben und Live-Sensordaten lesen
- Ermoeglicht KI-gestuetztes Debugging in Echtzeit
- Laeuft lokal auf dem Windows-Entwicklungsrechner, verbindet sich remote zum Pi

## Sicherheitsabgrenzung

- die Bridge empfaengt nur ESP-NOW — sie kann keine Pakete in den Steuerpfad einschleusen
- MQTT-Zugang ist passwortgeschuetzt (`allow_anonymous false` in Mosquitto)
- die Bridge authentifiziert sich mit eigenem MQTT-User/Passwort
- das Dashboard zeigt Daten an — es sendet keine Befehle an Controller, Receiver oder Arduino
- MQTT-Credentials liegen in gitignorierten `*.local.*`-Dateien
- der MQTT MCP Server nutzt eigene Read-Only-Credentials
- Mosquitto WebSocket laeuft hinter dem bestehenden Nginx HTTPS-Proxy

## OTA-Update (Over-The-Air Firmware-Upload)

Der Bridge-ESP32 unterstuetzt kabellosen Firmware-Upload per ArduinoOTA ueber WiFi.
Der MQTT MCP Server stellt ein `ota_flash`-Tool bereit, mit dem Claude die Bridge-Firmware kompilieren und kabellos flashen kann.

**ACHTUNG: OTA ist ein sicherheitskritischer Angriffsvektor.**
Unautorisierter Firmware-Upload bedeutet volle Kontrolle ueber den ESP32.

### OTA-Sicherheitskonzept

1. **Passwort-Pflicht** — OTA akzeptiert nur Uploads mit korrektem Passwort (aus gitignorierter `wifi_config.local.h`, getrennt von WiFi- und MQTT-Passwort)
2. **Nur lokales Netz** — kein Port-Forwarding, kein Internet-Zugang zum OTA-Port; OTA ist ausschliesslich im LAN erreichbar
3. **Firmware-Validierung** — der ESP32 prueft die Integritaet des empfangenen Binaries vor dem Flashen (Checksum, Magic Bytes)
4. **Dual-Partition mit Rollback** — alte Firmware bleibt in der zweiten OTA-Partition; bei Boot-Fehler (kein WiFi innerhalb 30s, kein MQTT-Connect innerhalb 60s) automatischer Rollback auf die vorherige Version
5. **Explizite User-Bestaetigung** — der MCP-Server fragt IMMER den User bevor er flasht; kein automatisches OTA ohne menschliche Freigabe
6. **Logging** — jeder OTA-Versuch (erfolgreich oder gescheitert) wird per MQTT auf `robotarm/ota/log` geloggt mit Zeitstempel, Quell-IP und Ergebnis
7. **Getrennte Credentials** — OTA-Passwort ist unabhaengig von MQTT-User/Passwort und WiFi-Passwort; drei separate Geheimnisse
8. **Nur Bridge** — OTA ist ausschliesslich auf dem Bridge-ESP32 aktiviert; Controller und Receiver haben KEIN OTA und KEIN WiFi; OTA darf NIEMALS auf Steuerpfad-Geraete ausgeweitet werden
9. **Kein OTA im Produktivbetrieb** — OTA ist ein reines Entwicklungswerkzeug; fuer den spaeteren Einsatz wird OTA deaktiviert oder physisch entfernt

### OTA-Ablauf

```
1. Claude kompiliert Firmware lokal (arduino-cli compile)
2. Claude fragt User: "Soll ich die Bridge-Firmware per OTA flashen?"
3. User bestaetigt explizit
4. MCP-Tool sendet .bin per HTTP an Bridge-ESP32 (ArduinoOTA, Passwort-geschuetzt)
5. Bridge validiert Binary, flasht auf zweite Partition, startet neu
6. Bridge prueft Boot-Kriterien (WiFi + MQTT erreichbar)
7. Bei Erfolg: neue Firmware bestaetigt, alte Partition wird freigegeben
8. Bei Fehler: automatischer Rollback auf vorherige Firmware
9. OTA-Ergebnis wird auf robotarm/ota/log geloggt
```

## MQTT Topic-Struktur

```
robotarm/
  imu              # ImuPaket v3 als JSON (20Hz, QoS 0, retained)
  status           # Bridge-Status: WiFi RSSI, Uptime, Fehlerrate (1Hz, QoS 1, retained)
  kalib            # Kalibrierungsstatus separat (bei Aenderung, QoS 1, retained)
  ota/log          # OTA-Versuchsprotokoll (bei jedem Versuch, QoS 1, nicht retained)
```

### Topic `robotarm/imu` — Payload

```json
{
  "z": 12345,
  "s": [
    {"h": 92.3, "r": 27.1, "p": 17.5},
    {"h": 359.9, "r": -46.1, "p": 4.2},
    {"h": 119.4, "r": 21.9, "p": 149.3}
  ],
  "k": [
    {"s": 3, "g": 3, "a": 3, "m": 3},
    {"s": 0, "g": 3, "a": 1, "m": 2},
    {"s": 0, "g": 3, "a": 1, "m": 2}
  ],
  "f": 42.5,
  "v": 3
}
```

Kompakte Schluessel um die Nachrichtengroesse bei 20Hz gering zu halten.
Vollstaendige Zuordnung: `z`=zaehler, `s`=sensoren, `h`=heading, `r`=roll, `p`=pitch,
`k`=kalib, `g`=gyro, `a`=accel, `m`=mag, `f`=flex_prozent, `v`=protokoll_version.

### Topic `robotarm/status` — Payload

```json
{
  "wifi_rssi": -42,
  "uptime_s": 3600,
  "pakete_gesamt": 72000,
  "fehler_gesamt": 12,
  "fehlerrate": 0.017,
  "pps": 19.8
}
```

## Konfiguration (gitignoriert)

| Datei | Ort | Inhalt |
|-------|-----|--------|
| `peer_config.local.h` | `firmware/esp32_bridge/` | Controller-MAC fuer Absendervalidierung |
| `wifi_config.local.h` | `firmware/esp32_bridge/` | WiFi SSID, Passwort, MQTT-Broker IP/Port/User/Passwort |
| `peer_config.local.h` | `firmware/esp32_controller/` | Receiver-MAC + Bridge-MAC + BRIDGE_AKTIV |
| `mqtt_config.local.py` | `dashboard/mcp/` | MQTT-Broker Adresse, Port, User, Passwort fuer MCP |
| `ota_config.local.py` | `dashboard/mcp/` | Bridge-IP, OTA-Port, OTA-Passwort fuer kabelloses Flashen |

## Recherchequellen

- [ESP-IDF ESP-NOW Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html) fuer die Koexistenz von ESP-NOW und WiFi-STA auf dem gleichen Kanal.
- [ESP32 WiFi + ESP-NOW Coexistence](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp-now) fuer die Bedingung, dass ESP-NOW und WiFi denselben Kanal teilen muessen.
- [Mosquitto MQTT Broker — WebSocket Support](https://mosquitto.org/man/mosquitto-conf-5.html) fuer die Konfiguration von WebSocket-Listenern.
- [mqtt.js — MQTT Client for Browser and Node](https://github.com/mqttjs/MQTT.js) fuer den Browser-seitigen MQTT-Client ueber WebSocket.
- [Three.js Documentation](https://threejs.org/docs/) fuer die 3D-Visualisierung des Roboterarms im Browser.
- [PubSubClient — Arduino MQTT Library](https://github.com/knolleary/pubsubclient) fuer den ESP32 MQTT-Client auf der Bridge.
- [Raspberry Pi Zero 2W Specifications](https://www.raspberrypi.com/products/raspberry-pi-zero-2-w/) fuer Hardware-Einschraenkungen (512MB RAM, BCM2710A1).
- [Model Context Protocol Specification](https://modelcontextprotocol.io/) fuer den MQTT MCP Server.
- [ESP32 OTA Updates — Arduino](https://docs.espressif.com/projects/arduino-esp32/en/latest/ota_web_update.html) fuer ArduinoOTA auf dem ESP32.
- [ESP32 Partition Table — OTA Data](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html) fuer Dual-Partition OTA mit Rollback.
