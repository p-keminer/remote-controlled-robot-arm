# ROS 2 Roboterarm — Schnellstart

Alle Befehle in einem **WSL-Terminal** ausfuehren.

## Stehend (Tischmontage)

```bash
./ros2/start_rviz.sh
```

## Wandmontage (horizontal, haengend)

```bash
./ros2/start_rviz_wall.sh
```

## Mit Live-MQTT-Daten (ESPs + Bridge muessen laufen)

```bash
ROS2_DIR="<pfad-zum-repo>/ros2"
export FASTRTPS_DEFAULT_PROFILES_FILE="$ROS2_DIR/fastdds_profile.xml"
source /opt/ros/jazzy/setup.bash
source "$ROS2_DIR/install/setup.bash"
ros2 launch robotarm_description display.launch.py
```

## Neu bauen (nach URDF-Aenderungen)

```bash
source /opt/ros/jazzy/setup.bash
cd <pfad-zum-repo>/ros2
colcon build --packages-select robotarm_description
```

## Hinweise

- VcXsrv muss NICHT laufen — WSLg reicht
- Fast-DDS nutzt Shared Memory (`fastdds_profile.xml`) — ohne die Datei funktioniert DDS nicht in WSL2
