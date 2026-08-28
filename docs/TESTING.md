<a id="top"></a>

<div align="center">

[![Deutsch](https://img.shields.io/badge/🇩🇪_Deutsch-24292f?style=for-the-badge)](#deutsch)
[![English](https://img.shields.io/badge/🇬🇧_English-24292f?style=for-the-badge)](#english)

</div>

---

<a id="deutsch"></a>
<a id="test--und-verifikationsstand"></a>
<a id="test-und-verifikationsstand"></a>

# Test- und Verifikationsstand

Dieses Dokument unterscheidet bestaetigte `Hardwaretests` von den frischen `Compile- und Statikchecks`. Ein gruener `Build` ersetzt weder einen Flash- noch einen Bewegungstest.

<a id="de-bestaetigte-hardwarepfade"></a>

## Bestaetigte Hardwarepfade

Die folgenden Aussagen stammen aus den Projektversuchen bis zum Prototyp-Freeze vom 2026-05-10; sie wurden beim Dokumentationsaudit vom 2026-08-28 nicht erneut auf Hardware ausgefuehrt:

- drei BNO055 am PCA9548A sowie Potentiometer-Greifereingabe am Controller wurden im realen Aufbau betrieben;
- `Controller -> ESP-NOW -> Receiver` ist als lokaler Funkpfad bestaetigt;
- `Receiver -> I2C -> Arduino -> fuenf Servos` wurde zuerst als Sweep und spaeter im Live-I2C-Modus getestet;
- der Live-I2C-Test umfasste Basis, Schulter, Ellbogen, Handgelenk und Greifer und wurde per Compile-Flags aktiviert; der Repo-Default blieb der Sweep;
- `Controller -> Bridge -> WiFi/MQTT -> Pi -> Dashboard/MCP/ROS` wurde als Debug-/Twin-Pfad end-to-end verifiziert.

Diese Nachweise belegen einen funktionierenden Prototyp, aber keine Produktiv-, Safety- oder Security-Freigabe.

<a id="de-frischer-softwarecheck"></a>

## Frischer Softwarecheck vom 2026-08-28

Toolchain und Abhaengigkeiten sind in [`BUILD.md`](BUILD.md#deutsch) festgehalten.

| Check | Ergebnis |
| --- | --- |
| Controller-Compile | erfolgreich, 930311 B Flash / 45544 B RAM |
| Receiver-Compile im Repo-Default | erfolgreich, 902395 B Flash / 45104 B RAM |
| Receiver-Compile Live-I2C mit allen Achsen | erfolgreich, 904751 B Flash / 45136 B RAM |
| Arduino-UNO-Compile | erfolgreich, 4280 B Flash / 282 B RAM |
| Controller-Hotloop-Guard | erfolgreich |
| Receiver-Live-I2C-Guard | erfolgreich |

Die Debug-Bridge wurde in diesem Audit nicht frisch kompiliert. Es wurde nichts geflasht und keine reale Bewegung ausgeloest.

<a id="de-guards-ausfuehren"></a>

## Guards ausfuehren

Vom Repository-Root:

```powershell
python -B tests/latency/controller_hotloop_guard.py
python -B tests/latency/receiver_live_i2c_guard.py
```

[`controller_hotloop_guard.py`](../tests/latency/controller_hotloop_guard.py) prueft statisch unter anderem:

- langsame BNO055-Status-/Kalibrierabfragen liegen nicht im schnellen Bewegungsloop;
- der Hotpath nutzt gecachten Kalibrierstatus;
- NVS-Offsets werden als effektiver Kalibrierstand behandelt;
- periodischer Serial-Debug und langsame Diagnose sind defaultmaessig aus;
- Einzelkalibrierung verwirft vorhandene Offsets nicht vorzeitig.

[`receiver_live_i2c_guard.py`](../tests/latency/receiver_live_i2c_guard.py) prueft statisch unter anderem:

- Sweep ist Default und Live-I2C ist deaktiviert;
- beide Receiver-Modi koennen nicht gleichzeitig aktiviert werden;
- 50-Hz-, 250-ms-Stale- und Slew-Konstanten sind vorhanden;
- der ESP-NOW-Callback sendet weder I2C noch Serial und puffert nur;
- Notaus und stale Daten werden auf E-Stop beziehungsweise Neutral abgebildet;
- Live-Mapping und achsweise Freigaben bleiben vorhanden.

Die Guards suchen erwartete Quelltextstrukturen. Sie sind Regressionstests, keine Laufzeit-, Funk-, Security- oder Hardwaremessung.

<a id="de-offene-hardwarefreigabe"></a>

## Vor einer erneuten Hardwarefreigabe offen

- Custom-FQBN aus einem frischen Clone reproduzierbar bereitstellen und den Build damit wiederholen;
- Debug-Bridge frisch kompilieren und ihren Start-/Reconnectpfad pruefen;
- I2C-Idlepegel und 3,3-V-Vertraeglichkeit am konkreten Aufbau messen;
- Notaus, 250-ms-Stale-Reaktion, 1-s-Arduino-Timeout und Neutralziele mit Zeitmessung pruefen;
- Servo-Limits, Stromaufnahme, Blockade, Last, Kabelzug und Dauerlauf testen;
- Funkstoerung, Controller-Neustart, Paketverlust und manipulierte Pakete testen;
- MQTT-ACL, MCP-Publishgrenze, OTA und Credential-Trennung gegen die reale Brokerkonfiguration pruefen;
- Steuerpfad und Debugpfad getrennt auf Ende-zu-Ende-Latenz vermessen.

Die Testuebersicht im Repository liegt in [`tests/README.md`](../tests/README.md). Safety- und Security-Grenzen stehen in [`SAFETY.md`](../SAFETY.md#deutsch) und [`SECURITY.md`](../SECURITY.md#deutsch).

<div align="center">

[![Nach oben](https://img.shields.io/badge/⬆_Nach_oben-24292f?style=for-the-badge)](#top)

</div>

---

<a id="english"></a>
<a id="test-and-verification-status"></a>

# Testing and verification status

This document distinguishes confirmed `hardware tests` from the fresh `compile and static checks`. A green `build` replaces neither a flash test nor a motion test.

<a id="en-confirmed-hardware-paths"></a>

## Confirmed hardware paths

The following statements originate from project trials up to the prototype freeze on 2026-05-10; they were not rerun on hardware during the documentation audit on 2026-08-28:

- three BNO055 sensors on the PCA9548A and the potentiometer gripper input on the controller were operated in the real setup;
- `Controller -> ESP-NOW -> Receiver` is confirmed as the local radio path;
- `Receiver -> I2C -> Arduino -> five servos` was tested first as a sweep and later in Live-I2C mode;
- the Live-I2C test covered base, shoulder, elbow, wrist, and gripper and was enabled through compile flags; the repository default remained the sweep;
- `Controller -> Bridge -> WiFi/MQTT -> Pi -> Dashboard/MCP/ROS` was verified end to end as the debug/twin path.

This evidence confirms a functional prototype, but it does not grant production, safety, or security approval.

<a id="en-fresh-software-check"></a>

## Fresh software check from 2026-08-28

The toolchain and dependencies are recorded in [`BUILD.md`](BUILD.md#english).

| Check | Result |
| --- | --- |
| Controller compile | successful, 930311 B flash / 45544 B RAM |
| Receiver compile with the repository default | successful, 902395 B flash / 45104 B RAM |
| Receiver compile, Live-I2C with all axes | successful, 904751 B flash / 45136 B RAM |
| Arduino UNO compile | successful, 4280 B flash / 282 B RAM |
| Controller hot-loop guard | successful |
| Receiver Live-I2C guard | successful |

The debug bridge was not freshly compiled in this audit. Nothing was flashed and no real motion was triggered.

<a id="en-running-the-guards"></a>

## Running the guards

From the repository root:

```powershell
python -B tests/latency/controller_hotloop_guard.py
python -B tests/latency/receiver_live_i2c_guard.py
```

[`controller_hotloop_guard.py`](../tests/latency/controller_hotloop_guard.py) statically checks, among other things, that:

- slow BNO055 status/calibration queries are not located in the fast motion loop;
- the hot path uses cached calibration status;
- NVS offsets are treated as an effective calibration state;
- periodic serial debug output and slow diagnostics are disabled by default;
- individual calibration does not discard existing offsets prematurely.

[`receiver_live_i2c_guard.py`](../tests/latency/receiver_live_i2c_guard.py) statically checks, among other things, that:

- sweep is the default and Live-I2C is disabled;
- both receiver modes cannot be enabled at the same time;
- the 50 Hz, 250 ms stale-data, and slew constants are present;
- the ESP-NOW callback sends neither I2C nor serial data and only buffers;
- emergency-stop and stale data are mapped to E-stop and neutral respectively;
- live mapping and per-axis enables remain present.

The guards look for expected source-code structures. They are regression tests, not runtime, radio, security, or hardware measurements.

<a id="en-open-hardware-approval"></a>

## Open before renewed hardware approval

- provide the custom FQBN reproducibly from a fresh clone and repeat the build with it;
- freshly compile the debug bridge and verify its startup/reconnect path;
- measure the I2C idle level and 3.3 V compatibility on the actual setup;
- verify the emergency stop, 250 ms stale-data reaction, 1 s Arduino timeout, and neutral targets with timing measurements;
- test servo limits, current draw, stall, load, cable pull, and endurance operation;
- test radio interference, controller restart, packet loss, and manipulated packets;
- verify the MQTT ACL, MCP publish boundary, OTA, and credential separation against the real broker configuration;
- measure the control path and debug path separately for end-to-end latency.

The repository test overview is located in [`tests/README.md`](../tests/README.md). Safety and security boundaries are documented in [`SAFETY.md`](../SAFETY.md#english) and [`SECURITY.md`](../SECURITY.md#english).

<div align="center">

[![Back to top](https://img.shields.io/badge/⬆_Back_to_top-24292f?style=for-the-badge)](#top)

</div>
