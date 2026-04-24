# calibration/gripper_input

## Zweck

Dieser Ordner dokumentiert den aktuellen Arbeitsstand der Greifer-Eingabe ueber ein Potentiometer.

## Aktueller Stand

Der Greifer nutzt aktuell ein Potentiometer aus Robustheitsgruenden.
Im aktuellen 2-Draht-Aufbau bleibt der 10k-Pull-Down auf dem Controller bestehen; das Potentiometer liefert den beobachteten Arbeitsbereich fuer das Protokollfeld `f`.

## Inhalt

- aktuelle Offen-/Zu-Anker des Potentiometers
- Hinweise zum 2-Draht-Aufbau auf `GPIO1`
- Bezug auf Dashboard-, ROS- und Controller-Mapping

## Regeln

- aktive Greifer-Aenderungen hier dokumentieren
- historische Flex-Sensor-Staende nur unter `../flex_sensor/` pflegen

## Schnittstellen / Abhaengigkeiten

- beeinflusst `../../firmware/esp32_controller/`
- beeinflusst `../../dashboard/` und `../../ros2/`
- ergaenzt `../../docs/measurement_logs/`
