# ROS 2 Roboterarm - Schnellstart

Alle Befehle in einem WSL-Terminal ausfuehren.

## Voraussetzungen

- ROS 2 Jazzy unter WSL installiert
- WSLg fuer Fenster wie RViz und Live-Monitor verfuegbar
- Workspace einmal gebaut:

```bash
source /opt/ros/jazzy/setup.bash
cd <pfad-zum-repo>/ros2
colcon build --packages-select robotarm_description
```

## Nur RViz starten

Tischmontage:

```bash
./ros2/start_rviz.sh
```

Wandmontage:

```bash
./ros2/start_rviz_wall.sh
```

## Live-Daten vom Pi-Broker

Wandaufbau mit Twin, MQTT-Bridge und Recorder:

```bash
./ros2/start_live_wall.sh
```

Wandaufbau mit Recorder plus Live-Monitor:

```bash
./ros2/start_live_wall_debug.sh
```

Vor jedem Start raeumen die Startskripte alte ROS-/RViz-/Monitor-Prozesse ueber `stop_live_wall_stack.sh` weg.

## Zielarm waehlen

Der Wandaufbau kennt drei Modi:

- `front` = linker Arm
- `back` = rechter Arm
- `both` = beide Arme gespiegelt

Beispiel fuer nur den rechten Arm:

```bash
source /opt/ros/jazzy/setup.bash
cd <pfad-zum-repo>/ros2
source install/setup.bash
ros2 launch robotarm_description debug_wall.launch.py target_arm:=back
```

## Debug-Session

Der Debug-Launch schreibt Sessions nach:

```text
ros2/debug_sessions/<timestamp>/
```

Inhalt:

- `raw_imu.jsonl`
- `mapping.jsonl`
- `joint_states.jsonl`
- `session.json`
- optional `curves.png`

## Replay einer Session

```bash
./ros2/start_replay_wall.sh ./ros2/debug_sessions/<timestamp>/raw_imu.jsonl
```

## Plot fuer eine Session

Neueste Session:

```bash
./ros2/plot_debug_session.sh
```

Gezielt:

```bash
./ros2/plot_debug_session.sh ./ros2/debug_sessions/<timestamp>
```

## MQTT-Konfiguration

Fuer Live-Daten wird lokal eine gitignorierte Datei auf Basis von `robotarm_mqtt.template.sh` genutzt.
Darin stehen Broker, User und Passwort fuer den Debugpfad.

## Aktueller Mapping-Stand

ROS bildet denselben Arbeitsstand wie das Dashboard ab:

- Basis aus `S2.pitch`
- Schulter aus `S2.heading`
- Ellenbogen aus relativer Rotation Unterarm zu Oberarm
- Wrist aus relativer Rotation Hand zu Unterarm
- Greifer aus `f` als Greifer-Prozentwert

Die Greifer-Eingabe kommt aktuell aus einem Potentiometer aus Robustheitsgruenden; das Protokollfeld `f` bleibt bestehen.

## Wichtige Hinweise

- ROS ist hier ein Debug- und Simulationspfad, kein Safety-freigegebener Bewegungscontroller.
- Das erfolgreiche ROS-Mapping bestaetigt den Twin, ersetzt aber keine Produktivfreigabe.
- Der aktuelle Live-Monitor ist absichtlich kompakt positioniert, damit die Fensterleiste unter WSL greifbar bleibt.
