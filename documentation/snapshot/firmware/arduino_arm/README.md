# firmware/arduino_arm

## Zweck

Dieser Ordner ist fuer die direkte Servoausfuehrung auf dem Adeept-Arduino reserviert.

## Aktueller Stand

Die Rolle des Arduino ist dokumentiert, die Implementierung folgt spaeter.
Die Servoebene ist als letzter Sicherheits- und Bewegungsanker definiert.
Die offizielle Produktbasis des Stock-Kits nennt das `Adeept Robotic Arm Driver Board` als `ATMEGA328P`-basierte Arduino-kompatible Steuerplatine; in der Arduino IDE ist dafuer `Arduino UNO R3` zu waehlen.
Der importierte offizielle V4.0-Download zeigt fuer die Stock-Laufzeitbasis fuenf aktive Servos auf `D9`, `D6`, `D5`, `D3` und `D11`; der sechste Boardport `D10/servo6` taucht im offiziellen Unpacking-Test als Reserve-/Testpfad auf.
Fuer die Originalfunktionen des Stock-Kits ist aktuell entschieden: Potentiometer-Modus bleibt Fallback, OLED wird uebernommen, Learning/Action Memory bleibt erhalten und die Processing-Steuerung bleibt vorerst als Referenz- und Fallbackpfad dokumentiert.

## Inhalt

- spaetere Servoinitialisierung und Pinzuordnung
- Abgleich zwischen Stock-Boardbasis und spaeterer Projektlogik
- Abgrenzung zwischen offiziellem 5-Servo-Laufzeitpfad und dem sechsten Stock-Testport
- Anwendung von Limits, Neutralwerten und Rampen
- UART-Empfang vom Receiver-ESP32
- Fehlerreaktion bei ausbleibenden oder ungueltigen Daten

## Regeln

- mechanische Limits duerfen nicht nur in externer Dokumentation existieren, sondern muessen in der Servoebene abgesichert sein
- Bewegungen werden nicht sprunghaft ohne Rampenbegrenzung ausgegeben
- Neutral- und Stopverhalten muessen mit `SAFETY_FRAMEWORK.md` und `calibration/servo_limits/` uebereinstimmen
- Herstellercode ist Referenz fuer den Stock-Zustand, aber nicht automatisch Projektzielcode
- der sechste Stock-Testport darf nicht ohne reale Aufbaupruefung als zusaetzliche Robotikachse behandelt werden

## Schnittstellen/Abhaengigkeiten

- empfaengt validierte Zielwerte aus `../esp32_receiver/`
- richtet sich fuer die serielle Uebergabe zusaetzlich nach `../UART_FRAME_V1.md`
- nutzt Limit- und Neutralregeln aus `../../calibration/servo_limits/`
- orientiert sich an `../../SAFETY_FRAMEWORK.md`

## Recherchequellen

- [Adeept 5-DOF Robotic Arm Kit Compatible with Arduino IDE](https://www.adeept.com/adeept-5-dof-robotic-arm-kit-compatible-with-arduino-ide-programmable-diy-coding-stem-educational-5-axis-robot-arm-with-oled-display-processing-code_p0360.html) fuer den Hinweis, dass das `Adeept Robotic Arm Driver Board` auf `ATMEGA328P` basiert und in der Arduino IDE als `Arduino UNO R3` zu behandeln ist.
- [Adeept V4 Package Review](../../official_downloads/ADEEPT_V4_PACKAGE_REVIEW.md) fuer die Einordnung von `Servo90`, `Unpacking_test_code` und der offiziellen 5-vs-6-Servo-Annahmen.
- [Servo90.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/_6%20Servo90/Servo90/Servo90.ino) fuer die offizielle 5-Servo-90-Grad-Basis auf `D9`, `D6`, `D5`, `D3`, `D11`.
- [AdeeptArmRobot.ino](../../official_downloads/extracted/adeept_v4_0_20251205/ADA031-Adeept_Robotic_Arm_Kit_for_Arduino-V4.0-20251205/Code/9.%20Processing/AdeeptArmRobot/AdeeptArmRobot.ino) fuer die offizielle 5-Servo-Laufzeitbasis des Processing-Steuerpfads.
