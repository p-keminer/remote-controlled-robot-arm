#!/bin/bash
set -e

echo "=== ROS 2 Jazzy Installation fuer Ubuntu 24.04 ==="
echo ""

# 1. Locale
echo "[1/7] Locale setzen..."
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# 2. ROS 2 Repository hinzufuegen
echo "[2/7] ROS 2 Repository hinzufuegen..."
sudo apt-get update -y
sudo apt-get install -y software-properties-common curl
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=amd64 signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu noble main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# 3. ROS 2 Jazzy Desktop installieren (inkl. RViz2)
echo "[3/7] ROS 2 Jazzy Desktop installieren (dauert ~10-20 Min)..."
sudo apt-get update -y
sudo apt-get install -y ros-jazzy-desktop

# 4. Dev Tools
echo "[4/7] Build-Tools installieren..."
sudo apt-get install -y python3-colcon-common-extensions python3-rosdep python3-pip

# 5. rosdep initialisieren
echo "[5/7] rosdep initialisieren..."
sudo rosdep init 2>/dev/null || true
rosdep update

# 6. MQTT und Robotik-Pakete
echo "[6/7] MQTT Bridge und Robotik-Pakete installieren..."
sudo apt-get install -y     ros-jazzy-joint-state-publisher     ros-jazzy-joint-state-publisher-gui     ros-jazzy-robot-state-publisher     ros-jazzy-xacro     ros-jazzy-rviz2
pip3 install paho-mqtt --break-system-packages 2>/dev/null || pip3 install paho-mqtt

# 7. Source in bashrc
echo "[7/7] ROS 2 in .bashrc eintragen..."
if ! grep -q "ros/jazzy" ~/.bashrc; then
    echo "" >> ~/.bashrc
    echo "# ROS 2 Jazzy" >> ~/.bashrc
    echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc
    echo "ROS 2 source-Eintrag in .bashrc hinzugefuegt"
else
    echo "ROS 2 bereits in .bashrc"
fi

echo ""
echo "=== Installation abgeschlossen ==="
echo "Starte eine neue Shell oder fuehre aus:"
echo "  source /opt/ros/jazzy/setup.bash"
echo "Test mit:"
echo "  ros2 topic list"
