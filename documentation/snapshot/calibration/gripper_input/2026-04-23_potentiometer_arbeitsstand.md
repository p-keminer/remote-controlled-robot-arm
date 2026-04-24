# Potentiometer-Arbeitsstand 2026-04-23

## Kontext

- Datum: 2026-04-23
- Ort im System: Controller `GPIO1`
- Aufbau: 2-Draht-Arbeitsstand mit bestehendem 10k-Pull-Down auf dem Controller
- Zweck: aktueller robuster Greifer-Pfad fuer Dashboard, ROS und Live-Debugging

## Verdrahtung

- bestehende `3.3V`-Leitung auf einen aeusseren Poti-Pin
- bestehende ADC-/Teiler-Leitung auf den mittleren Pin (Wischer)
- 10k-Pull-Down gegen GND bleibt auf dem Controller bestehen

## Arbeitsanker

| Zustand | ADC-Wert | Bedeutung |
| --- | ---: | --- |
| offen / Ruhelage | 1935 | soll im Mapping Richtung `0%` laufen |
| geschlossen / zu | 3020 | soll im Mapping Richtung `100%` laufen |

## Einordnung

- das Protokollfeld `f` bleibt aus Rueckwaertskompatibilitaet erhalten
- Dashboard und ROS mappen diesen Arbeitsbereich aktuell direkt auf den Greifer
- der Potentiometer-Pfad ist der aktive Stand; der alte Flex-Sensor-Pfad bleibt nur noch historisch erhalten
