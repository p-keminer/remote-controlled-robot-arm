# Servo-Pin-Zuordnung

Diese Datei bleibt als stabiles Ziel fuer Verweise im aktiven und archivierten
Arduino-Sketch erhalten. Die kanonische Hardwarebeschreibung steht in
[`docs/HARDWARE.md`](../docs/HARDWARE.md).

| Gelenk | Arduino-Pin | zugeordneter Poti-Eingang |
| --- | ---: | ---: |
| Basis | D9 | A0 |
| Greifer | D6 | A1 |
| Handgelenk | D5 | A2 |
| Ellbogen | D3 | A3 |
| Schulter | D11 | A6 |

Diese empirisch ermittelte Zuordnung weicht vom Herstellerbeispiel ab. Eine
Aenderung erfordert eine erneute Pruefung an der realen Verkabelung.

Gemessene Winkelgrenzen und sichere Zielwerte stehen in
[`calibration/servo_limits/README.md`](../calibration/servo_limits/README.md).
