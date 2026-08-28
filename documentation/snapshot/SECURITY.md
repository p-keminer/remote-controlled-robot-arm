<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>
<a id="security"></a>

# Security

Das Repository beschreibt einen lokal betriebenen Prototyp. Ein funktionierender Steuer- oder Debugpfad ist keine Security-Freigabe. Security behandelt unautorisierte Daten und Zugaenge; physische Bewegungsfolgen behandelt separat [`SAFETY.md`](SAFETY.md#deutsch).

<a id="de-vertrauensgrenzen"></a>

## Vertrauensgrenzen

| Bereich | Pfad | Aktueller Zweck |
| --- | --- | --- |
| Steuerung | `Controller -> ESP-NOW -> Receiver -> I2C -> Arduino` | bewegungsrelevant |
| Debug/Twin | `Controller -> ESP-NOW -> Bridge -> WiFi/MQTT -> Dashboard, MCP, ROS` | Beobachtung, Diagnose, Replay und Twin |
| Lokale Wartung | USB/Serial sowie OTA der Bridge | Entwicklung und Provisioning |

Die Bridge liegt nicht im Steuerpfad. MQTT, Dashboard, MCP und ROS koennen im aktuellen Stand keine Servo-Sollwerte an Receiver oder Arduino senden. Manipulierte Debugdaten koennen dennoch Diagnose, Twin, Logs und menschliche Entscheidungen taeuschen und sind deshalb nicht harmlos.

<a id="de-schutz-und-luecken"></a>

## Aktueller Schutz und bekannte Luecken

<a id="de-esp-now-steuerpfad"></a>

### ESP-NOW-Steuerpfad

- Der Controller sendet per Unicast auf Kanal 1 an lokal konfigurierte Peer-Adressen.
- ESP-NOW-Verschluesselung ist fuer die aktiven Peers ausgeschaltet (`encrypt=false`).
- Der Receiver validiert Paketlaenge, Protokollversion, XOR-Pruefsumme und steigenden Zaehler.
- Der Receiver besitzt derzeit keine Quell-MAC-Allowlist im Empfangs-Callback.
- XOR erkennt einfache Uebertragungsfehler, authentisiert aber keinen Absender.
- Es gibt noch keine `session_id`, keinen Authentisierungstag und keinen belastbaren Replay-Schutz ueber Neustarts hinweg.

Fuer eine spaetere ESP-NOW-Verschluesselung muessen PMK und eine LMK pro Peer explizit provisioniert werden. Ohne gesetzte LMK bleibt der Unicast-Frame unverschluesselt; ohne gesetzte PMK verwendet ESP-IDF eine Default-PMK. Verschluesselter Multicast wird nicht unterstuetzt. Der Bewegungsweg soll daher bei dedizierten Unicast-Peers bleiben und darf sich nicht allein auf MAC-Adressen oder Link-Layer-Verschluesselung verlassen.

Damit ist der Funkpfad fuer einen lokalen Prototyp funktionsfaehig, aber nicht gegen einen aktiven Angreifer freigegeben. Details stehen in [`docs/PROTOCOL.md`](docs/PROTOCOL.md#deutsch).

<a id="de-i2c-und-servoebene"></a>

### I2C und Servoebene

I2C ist eine lokale interne Busverbindung ohne Authentisierung. Startbytes, Version und XOR schuetzen nur gegen einfache Framefehler. Der Arduino begrenzt Winkel, Rampe und Timeout, aber diese Safety-Reaktionen ersetzen keine Security des Funkpfads und keine elektrische Leistungsabschaltung.

<a id="de-debug-bridge-mqtt-und-ota"></a>

### Debug-Bridge, MQTT und OTA

- Die Bridge prueft die lokal konfigurierte Controller-MAC sowie Version und XOR, bevor sie Daten publiziert.
- WiFi-, MQTT- und OTA-Zugangsdaten liegen in einer lokalen Konfiguration; die drei Passwoerter muessen verschieden sein.
- MQTT nutzt im aktuellen Quellstand Port `1883` ohne TLS. Vertraulichkeit und Serverauthentisierung sind damit nicht aus dem Transport ableitbar.
- Die ACL-Vorlage gibt der Bridge Schreibrecht auf `robotarm/#` und Dashboard/`mcp_reader` Leserecht auf `robotarm/#`. Fuer reale Broker sollten Schreibrechte auf die tatsaechlich benoetigten Bridge-Topics begrenzt werden.
- OTA ist ein eigener privilegierter Zugang zur Bridge und gehoert nur in ein vertrauenswuerdiges lokales Netz. Ein OTA-Erfolg sagt nichts ueber den Steuerpfad aus.

<a id="de-mcp-nicht-read-only"></a>

## MCP ist nicht rein read-only

[`dashboard/mcp/mqtt_mcp_server.py`](dashboard/mcp/mqtt_mcp_server.py) stellt neben Leseoperationen auch `mqtt_publish` bereit. Das Tool erlaubt im Code Publishes auf beliebige Topics unter `robotarm/`, einschliesslich retained Messages.

Mit der committed [`mosquitto_acl.template.conf`](dashboard/mcp/mosquitto_acl.template.conf) meldet sich MCP als `mcp_reader` an und hat brokerseitig nur Leserecht; der Publishversuch sollte dann abgewiesen werden. Diese Eigenschaft kommt von der Broker-ACL, nicht vom MCP-Code. Fehlt die ACL oder ist sie breiter, kann MCP Debug-/Twin-Topics beschreiben. Deshalb gilt:

- MCP nie allein aufgrund seines Benutzernamens als read-only einstufen;
- effektive ACL auf dem realen Broker pruefen;
- Publish-Tests nur auf einem expliziten Testtopic und nie mit fremden retained Daten durchfuehren;
- keine zukuenftige Bewegungs- oder Service-Schnittstelle unter einer pauschalen `robotarm/#`-Schreibfreigabe betreiben.

<a id="de-secrets"></a>

## Secrets und lokale Konfiguration

Echte MACs, SSIDs, IPs, Benutzernamen, Passwoerter, Schluessel und lokale Pfade gehoeren nicht in Git. Committed werden nur Templates; reale Werte liegen in `*.local.*`-Dateien:

- Controller: `firmware/esp32_controller/peer_config.local.h`
- Bridge: `firmware/esp32_bridge/peer_config.local.h` und `wifi_config.local.h`
- MQTT/MCP: `dashboard/mcp/mqtt_config.local.py`
- OTA-Tooling: `dashboard/mcp/ota_config.local.py`
- lokale Geraete-/Freigabedaten: `security/local/`

Die meisten dieser Pfade sind in [`.gitignore`](.gitignore) abgedeckt. Eine bekannte Ausnahme ist die von den ROS-Startskripten erwartete Datei `ros2/robotarm_mqtt.local.sh`: Sie ist derzeit nicht durch die committed `.gitignore` ausgeschlossen. Bis eine separate Config-Aenderung freigegeben wird, muss sie lokal ueber `.git/info/exclude` ausgeschlossen und vor jedem Commit mit `git status` kontrolliert werden.

Vor Commit oder Push:

```powershell
git status --short
tr -d '\r' < scripts/secret_scan.sh | bash -s -- --staged
tr -d '\r' < scripts/secret_scan.sh | bash -s -- --tracked
```

Der Scanner ist eine zusaetzliche Schranke, kein Beweis dafuer, dass keine Secrets vorhanden sind. Vorlagen duerfen nur erkennbare Platzhalter enthalten. Falls ein echter Wert jemals committed wurde, reicht spaeteres Loeschen aus dem Arbeitsbaum nicht; der Wert muss rotiert und der Git-Verlauf separat bewertet werden.

<a id="de-security-freigabe"></a>

## Mindestpruefungen vor einer Security-Freigabe

- Receiver-Absenderbindung und ESP-NOW-Verschluesselung beziehungsweise applikationsseitige Authentisierung entwerfen und testen;
- Session-/Replay-Verhalten inklusive Controller-Neustart pruefen;
- MQTT-Broker mit realer ACL, getrennten Konten und nach Moeglichkeit verschluesseltem Transport testen;
- MCP-Publishfaehigkeit und retained Messages als schreibende Debugoberflaeche behandeln;
- OTA-Authentisierung, Recovery und Credential-Rotation pruefen;
- Debug-/Twin-Ausfall und manipulierte Telemetrie testen, ohne daraus Bewegungsfreigaben abzuleiten;
- Ergebnisse reproduzierbar nach [`docs/TESTING.md`](docs/TESTING.md#deutsch) protokollieren.

<a id="de-primaerquellen"></a>

## Primaerquellen und Advisory-Pruefung

- [ESP-IDF: ESP-NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/network/esp_now.html) fuer PMK, LMK, CCMP und Multicast-Grenzen
- [Security Advisories von ESP-IDF](https://github.com/espressif/esp-idf/security/advisories)
- [Security Advisories der optionalen `espressif/esp-now`-Komponente](https://github.com/espressif/esp-now/security/advisories)

Vor einer Bewegungsfreigabe ist die Advisory-Lage der tatsaechlich verwendeten Arduino-ESP32-/ESP-IDF-Basis zu pruefen. Ein Advisory der optionalen `espressif/esp-now`-Komponente gilt nicht automatisch fuer den ESP-IDF-internen ESP-NOW-Stack; die konkrete Abhaengigkeit und betroffene Version muessen zuerst nachgewiesen werden.

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>
<a id="security-english"></a>

# Security

The repository describes a locally operated prototype. A functional control or debug path is not a security approval. Security addresses unauthorized data and access; physical motion consequences are addressed separately in [`SAFETY.md`](SAFETY.md#english).

<a id="en-trust-boundaries"></a>

## Trust boundaries

| Area | Path | Current purpose |
| --- | --- | --- |
| Control | `Controller -> ESP-NOW -> Receiver -> I2C -> Arduino` | motion-relevant |
| Debug/twin | `Controller -> ESP-NOW -> Bridge -> WiFi/MQTT -> Dashboard, MCP, ROS` | observation, diagnostics, replay, and twin |
| Local maintenance | USB/serial and bridge OTA | development and provisioning |

The bridge is not in the control path. In the current state, MQTT, the dashboard, MCP, and ROS cannot send servo targets to the receiver or Arduino. Manipulated debug data can nevertheless deceive diagnostics, the twin, logs, and human decisions and is therefore not harmless.

<a id="en-protection-and-gaps"></a>

## Current protection and known gaps

<a id="en-esp-now-control-path"></a>

### ESP-NOW control path

- The controller sends by unicast on channel 1 to locally configured peer addresses.
- ESP-NOW encryption is disabled for the active peers (`encrypt=false`).
- The receiver validates packet length, protocol version, XOR checksum, and an increasing counter.
- The receiver currently has no source-MAC allowlist in its receive callback.
- XOR detects simple transmission errors but does not authenticate a sender.
- There is no `session_id`, authentication tag, or robust replay protection across restarts yet.

For future ESP-NOW encryption, a PMK and one LMK per peer must be provisioned explicitly. Without a configured LMK, the unicast frame remains unencrypted; without a configured PMK, ESP-IDF uses a default PMK. Encrypted multicast is not supported. The motion path should therefore remain on dedicated unicast peers and must not rely solely on MAC addresses or link-layer encryption.

The radio path is therefore functional for a local prototype, but it is not approved against an active attacker. Details are documented in [`docs/PROTOCOL.md`](docs/PROTOCOL.md#english).

<a id="en-i2c-and-servo-layer"></a>

### I2C and servo layer

I2C is a local internal bus connection without authentication. Start bytes, version, and XOR protect only against simple frame errors. The Arduino limits angles, ramp, and timeout, but these safety reactions replace neither radio-path security nor an electrical power disconnect.

<a id="en-debug-bridge-mqtt-and-ota"></a>

### Debug bridge, MQTT, and OTA

- The bridge checks the locally configured controller MAC as well as version and XOR before publishing data.
- WiFi, MQTT, and OTA credentials are stored in a local configuration; the three passwords must be different.
- In the current source state, MQTT uses port `1883` without TLS. Confidentiality and server authentication therefore cannot be inferred from the transport.
- The ACL template grants the bridge write access to `robotarm/#` and the dashboard/`mcp_reader` read access to `robotarm/#`. On real brokers, write permissions should be restricted to the bridge topics that are actually required.
- OTA is a separate privileged access path to the bridge and belongs only in a trusted local network. OTA success says nothing about the control path.

<a id="en-mcp-not-read-only"></a>

## MCP is not purely read-only

In addition to read operations, [`dashboard/mcp/mqtt_mcp_server.py`](dashboard/mcp/mqtt_mcp_server.py) exposes `mqtt_publish`. In code, the tool allows publishes to arbitrary topics below `robotarm/`, including retained messages.

With the committed [`mosquitto_acl.template.conf`](dashboard/mcp/mosquitto_acl.template.conf), MCP signs in as `mcp_reader` and has broker-side read permission only; the publish attempt should then be rejected. This property comes from the broker ACL, not from the MCP code. If the ACL is missing or broader, MCP can write debug/twin topics. Therefore:

- never classify MCP as read-only based only on its username;
- verify the effective ACL on the real broker;
- run publish tests only on an explicit test topic and never with someone else's retained data;
- do not operate any future motion or service interface under blanket `robotarm/#` write permission.

<a id="en-secrets"></a>

## Secrets and local configuration

Real MACs, SSIDs, IPs, usernames, passwords, keys, and local paths do not belong in Git. Only templates are committed; real values are stored in `*.local.*` files:

- Controller: `firmware/esp32_controller/peer_config.local.h`
- Bridge: `firmware/esp32_bridge/peer_config.local.h` and `wifi_config.local.h`
- MQTT/MCP: `dashboard/mcp/mqtt_config.local.py`
- OTA tooling: `dashboard/mcp/ota_config.local.py`
- local device/approval data: `security/local/`

Most of these paths are covered by [`.gitignore`](.gitignore). One known exception is `ros2/robotarm_mqtt.local.sh`, which is expected by the ROS startup scripts: it is currently not excluded by the committed `.gitignore`. Until a separate configuration change is approved, it must be excluded locally through `.git/info/exclude` and checked with `git status` before every commit.

Before committing or pushing:

```powershell
git status --short
tr -d '\r' < scripts/secret_scan.sh | bash -s -- --staged
tr -d '\r' < scripts/secret_scan.sh | bash -s -- --tracked
```

The scanner is an additional barrier, not proof that no secrets are present. Templates may contain recognizable placeholders only. If a real value was ever committed, later deletion from the working tree is not sufficient; the value must be rotated and the Git history assessed separately.

<a id="en-security-approval"></a>

## Minimum checks before security approval

- design and test receiver sender binding and ESP-NOW encryption or application-level authentication;
- verify session/replay behavior, including controller restarts;
- test the MQTT broker with the real ACL, separate accounts, and encrypted transport where possible;
- treat MCP publish capability and retained messages as a writable debug interface;
- verify OTA authentication, recovery, and credential rotation;
- test debug/twin failure and manipulated telemetry without deriving motion approvals from them;
- record results reproducibly according to [`docs/TESTING.md`](docs/TESTING.md#english).

<a id="en-primary-sources"></a>

## Primary sources and advisory review

- [ESP-IDF: ESP-NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/network/esp_now.html) for PMK, LMK, CCMP, and multicast limitations
- [ESP-IDF security advisories](https://github.com/espressif/esp-idf/security/advisories)
- [Security advisories for the optional `espressif/esp-now` component](https://github.com/espressif/esp-now/security/advisories)

Before motion approval, the advisory status of the Arduino-ESP32/ESP-IDF base that is actually used must be checked. An advisory for the optional `espressif/esp-now` component does not automatically apply to ESP-IDF's internal ESP-NOW stack; the specific dependency and affected version must first be demonstrated.

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
