# ROS 2 Roboterarm — Schnellstart

Alle Befehle in einem **WSL-Terminal** ausfuehren.

## Stehend (Tischmontage)

```bash
~/projects/embedded/robotic-arm/ros2/start_rviz.sh
```

## Wandmontage (horizontal, haengend)

```bash
~/projects/embedded/robotic-arm/ros2/start_rviz_wall.sh
```

## Mit Live-MQTT-Daten (ESPs + Bridge muessen laufen)

```bash
export FASTRTPS_DEFAULT_PROFILES_FILE=~/projects/embedded/robotic-arm/ros2/fastdds_profile.xml
source /opt/ros/jazzy/setup.bash
source ~/projects/embedded/robotic-arm/ros2/install/setup.bash
ros2 launch robotarm_description display.launch.py
```

## Neu bauen (nach URDF-Aenderungen)

```bash
source /opt/ros/jazzy/setup.bash
cd ~/projects/embedded/robotic-arm/ros2
colcon build --packages-select robotarm_description
```

## Hinweise

- VcXsrv muss NICHT laufen — WSLg reicht
- Fast-DDS nutzt Shared Memory (`fastdds_profile.xml`) — ohne die Datei funktioniert DDS nicht in WSL2
