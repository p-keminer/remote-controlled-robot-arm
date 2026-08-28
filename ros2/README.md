# ROS 2 Digital Twin

Dieses Verzeichnis enthaelt den ROS-2-/RViz-Digital-Twin fuer den Adeept
5-DOF-Arm. Er liest Telemetrie aus dem MQTT-Debugpfad, berechnet
Gelenkpositionen und visualisiert oder protokolliert sie. Er steuert keine
Servos und ist kein Safety-Pfad.

```text
Bridge --MQTT robotarm/imu--> mqtt_bridge
       --ROS /joint_states--> robot_state_publisher --> RViz
                            \-> Recorder / Live-Monitor / Replay
```

## Inhalt

| Pfad | Aufgabe |
| --- | --- |
| `src/robotarm_description/urdf/` | Tisch- und Wandmodell |
| `src/robotarm_description/meshes/` | eingecheckte STL-Geometrien |
| `src/robotarm_description/launch/` | RViz-, Live-, Debug- und Replay-Launches |
| `src/robotarm_description/robotarm_description/mapping.py` | Twin-Mapping |
| `.../mqtt_bridge.py` | MQTT-Eingang und ROS-Publisher |
| `.../debug_recorder.py` | JSONL-Aufzeichnung einer Debug-Session |
| `.../imu_replayer.py` | zeitgetreues Replay von `raw_imu.jsonl` |
| `.../live_monitor.py` | Matplotlib-Livekurven |
| `.../debug_plot.py` | PNG-Kurven aus einer Session |
| `start_*.sh`, `plot_*.sh`, `stop_*.sh` | Start- und Diagnosehelfer |

## Voraussetzungen und Build

Der verifizierte Zielkontext ist ROS 2 Jazzy unter WSL mit WSLg. Benoetigt
werden mindestens:

- ROS-Pakete fuer `rclpy`, `sensor_msgs`, `std_msgs`, `std_srvs`,
  `robot_state_publisher`, `joint_state_publisher_gui` und `rviz2`
- Python-Pakete `paho-mqtt` und `matplotlib`
- ein Tk-Backend fuer den Live-Monitor
- `numpy` und `trimesh` nur, wenn die eingecheckten STL-Dateien neu erzeugt
  werden sollen

Build ab dem Verzeichnis `ros2/`:

```bash
cd <pfad-zum-repo>/ros2
source /opt/ros/jazzy/setup.bash
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install --packages-select robotarm_description
source install/setup.bash
```

Vor einem Live-Start muessen sich `paho.mqtt.client` und fuer den Debug-Start
auch `matplotlib` im selben Python-Kontext wie ROS importieren lassen.

## MQTT konfigurieren

Die Startskripte laden optional eine lokale, gitignorierte Datei:

```bash
cp robotarm_mqtt.template.sh robotarm_mqtt.local.sh
chmod 600 robotarm_mqtt.local.sh
```

| Variable | Standard ohne lokale Datei | Bedeutung |
| --- | --- | --- |
| `ROBOTARM_MQTT_HOST` | `localhost` | Mosquitto-Host |
| `ROBOTARM_MQTT_PORT` | `1883` | MQTT-Port |
| `ROBOTARM_MQTT_USER` | `mcp_reader` | nur lesender Broker-Account |
| `ROBOTARM_MQTT_PASS` | leer | lokales Passwort |
| `ROBOTARM_TARGET_ARM` | `both` | `front`, `back` oder `both` |
| `ROBOTARM_RECORD_DEBUG` | `true` | Recorder im Debug-Launch |
| `ROBOTARM_SHOW_LIVE_MONITOR` | `true` | Live-Monitor im Debug-Launch |
| `ROBOTARM_DEBUG_DIR` | `ros2/debug_sessions` | lokale Session-Ausgabe |

Passwoerter und lokale Hosts werden nur in `robotarm_mqtt.local.sh` gepflegt.
Die Datei darf nicht eingecheckt werden.

## Starten

Alle folgenden Befehle gelten **nach** `cd <pfad-zum-repo>/ros2`; deshalb
enthalten die Pfade kein zusaetzliches `./ros2/`.

Nur RViz mit Gelenkslidern, ohne MQTT:

```bash
./start_rviz.sh          # Tischmodell
./start_rviz_wall.sh     # zwei gespiegelte Wandarme
```

Live-Twin aus `robotarm/imu`:

```bash
./start_live_wall.sh
```

Live-Twin mit Recorder und optionalem Matplotlib-Monitor:

```bash
./start_live_wall_debug.sh
```

Die beiden Live-Skripte rufen vorher `stop_live_wall_stack.sh` auf. Dieses
Skript beendet passende ROS-/RViz-/Monitor-Prozesse mit `kill -9` und sollte
daher nur in der dafuer vorgesehenen WSL-Sitzung verwendet werden.

Der Wand-Launch kennt drei Zielmodi:

- `front`: nur linker Arm
- `back`: nur rechter Arm
- `both`: beide gespiegelten Arme

Zur Laufzeit wechselt der Dienst zyklisch `front -> back -> both`:

```bash
ros2 service call /toggle_arm std_srvs/srv/Trigger '{}'
```

## Debug-Session, Replay und Plot

Der Debug-Launch schreibt standardmaessig nach
`debug_sessions/<YYYYMMDD_HHMMSS>/`:

- `raw_imu.jsonl`
- `mapping.jsonl`
- `joint_states.jsonl`
- `session.json`
- optional `curves.png`

`debug_sessions/` ist gitignoriert. Vor einem Replay sollte ein laufender
Live-Stack explizit beendet werden:

```bash
./stop_live_wall_stack.sh
./start_replay_wall.sh debug_sessions/<timestamp>/raw_imu.jsonl
./start_replay_wall.sh debug_sessions/<timestamp>/raw_imu.jsonl 0.5
```

Plot der neuesten oder einer bestimmten Session:

```bash
./plot_debug_session.sh
./plot_debug_session.sh debug_sessions/<timestamp>
```

## Schnittstellen

### MQTT-Eingang

Der ROS-Node `mqtt_bridge` abonniert nur `robotarm/imu`. Die MQTT-Topics
`robotarm/status`, `robotarm/kalib` und `robotarm/ota/log` werden von diesem
Paket nicht konsumiert.

### ROS-2-Ausgabe

| ROS-Name | Typ | Erzeuger | Aktiv wenn |
| --- | --- | --- | --- |
| `/joint_states` | `sensor_msgs/JointState` | MQTT-Bridge oder Replay | Live/Replay |
| `/robotarm/debug/raw_imu` | `std_msgs/String` | MQTT-Bridge oder Replay | `publish_debug=true` |
| `/robotarm/debug/mapping` | `std_msgs/String` | MQTT-Bridge oder Replay | `publish_debug=true` |
| `robot_description` | Node-Parameter | Launch-Datei | jeder Launch |
| `/tf`, `/tf_static` | TF | `robot_state_publisher` | jeder Launch |
| `/toggle_arm` | `std_srvs/Trigger` | MQTT-Bridge | Live |

Die Namen mit fuehrendem `/` sind ROS-2-Namen und duerfen nicht als
MQTT-Topics dokumentiert oder in Mosquitto abonniert werden.

## Twin-Mapping

`mapping.py` und `dashboard/web/js/robotarm_3d.js` verwenden denselben
Twin-Stand:

| Gelenk | Twin-Eingang |
| --- | --- |
| Basis | `88.9 - S2.pitch`, visuell `+90..-90` Grad |
| Schulter | negierte Heading-Abweichung von `S2` zu `280.8` Grad |
| Ellbogen | relative Heading-Abweichung `S1` zu `S2` |
| Wrist | negierte relative Roll-Abweichung `S0` zu `S1`, Offset und Stabilisierung |
| Greifer | `f` in Prozent, invertiert auf die visuelle Oeffnung und geglaettet |

Dieses Twin-Mapping ist nicht identisch mit dem Realarm-Mapping in
[`live_i2c_mapping.h`](../firmware/esp32_receiver/live_i2c_mapping.h). Dort
weichen unter anderem Basisrichtung, Schulterreferenz/-fenster und die
Wrist-Fallback-/Nullungslogik ab. Konstanten duerfen deshalb nicht ungeprueft
zwischen Twin und Receiver kopiert werden.

## Test und Diagnose

Pakettests nach einem Build:

```bash
colcon test --packages-select robotarm_description
colcon test-result --verbose
```

Schneller Laufzeitcheck bei gestartetem Live-Stack:

```bash
ros2 topic echo --once /joint_states
ros2 topic hz /joint_states
ros2 topic echo --once /robotarm/debug/mapping
```

Der letzte Befehl liefert nur im Debug-Launch Daten. Bei DDS-Problemen kann
`test_dds.template.sh` nach `test_dds.local.sh` kopiert und der lokale Pfad
angepasst werden.

## Bekannte Grenzen

- Das Paket ist ein Debug-/Simulationspfad und besitzt keinen Servo-Ausgang.
- `fastdds_profile.xml` erzwingt Shared Memory und ist fuer Prozesse auf
  demselben WSL-Host gedacht, nicht fuer verteilte ROS-Rechner.
- Der Live-Pfad stabilisiert Wrist und Greifer zustandsbehaftet. Das Replay
  nutzt zwar dasselbe Grundmapping, wendet diese Live-Stabilisierung aber
  derzeit nicht an und ist daher nicht samplegenau identisch.
- `mapping.py` publiziert den Wrist-Zwischenwert als `roll_rel_deg`; Plot und
  Live-Monitor suchen aktuell noch `twist_rel_deg`. Die betreffende
  Zwischenwert-Kurve bleibt dadurch leer, waehrend Gelenkwinkel und
  `/joint_states` weiter funktionieren.
- Die Standard-Lintertests decken Stil ab, aber keine numerischen
  Mapping-Grenzfaelle.

Weitere Details stehen in
[`dashboard/README.md`](../dashboard/README.md),
[`docs/PROTOCOL.md`](../docs/PROTOCOL.md),
[`docs/TESTING.md`](../docs/TESTING.md) und
[`SECURITY.md`](../SECURITY.md).
