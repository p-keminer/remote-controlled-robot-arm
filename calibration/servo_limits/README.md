# Servo-Grenzen

Diese Seite bleibt als stabiles Ziel fuer Verweise in den Arduino-Sketches
erhalten. Die vollstaendige aktive Kalibrierung steht in
[`calibration/README.md`](../README.md); Verdrahtung und Sicherheitsgrenzen
stehen in [`docs/HARDWARE.md`](../../docs/HARDWARE.md) und
[`SAFETY.md`](../../SAFETY.md).

| Gelenk | Pin | Min | Max | Start / Timeout / Software-Stop |
| --- | ---: | ---: | ---: | ---: |
| Basis | D9 | 12 Grad | 139 Grad | 75 Grad |
| Schulter | D11 | 35 Grad | 142 Grad | 88 Grad |
| Ellbogen | D3 | 80 Grad | 175 Grad | 127 Grad |
| Handgelenk | D5 | 5 Grad | 177 Grad | 91 Grad |
| Greifer | D6 | 32 Grad | 126 Grad | 79 Grad |

Die Werte wurden am Prototyp empirisch ermittelt. Sie sind keine
zertifizierten Sicherheitsgrenzen. Insbesondere ist `90 Grad` nicht die aktive
Rueckfallpose aller Achsen. Aenderungen erfordern eine erneute mechanische
Pruefung am realen Arm.
