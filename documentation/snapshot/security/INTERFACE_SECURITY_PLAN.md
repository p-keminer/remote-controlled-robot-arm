# Interface Security Plan

Dieses Dokument beschreibt die Sicherheitsregeln fuer Service-, Debug- und Datenschnittstellen.

## Relevante Schnittstellen

- ESP32-Programmier- und Debugzugriff
- I2C zwischen Receiver und Arduino
- MQTT-/ROS-/Dashboard-Zugang als Debugpfad

## Aktuelle Regeln

- Debug- und Beobachtungsschnittstellen bleiben getrennt von der Bewegungsausgabe.
- I2C bleibt eine klar definierte Bewegungsuebergabe, keine offene Befehlsschnittstelle.
- WiFi/MQTT bleibt lesend und darf keine versteckte Ruecksteuerung einfuehren.
