# Projekt Fortschritt

Dieses Dokument fuehrt den aktuellen Managementstand des Projekts.

## Gesamtstatus

- Projektmodus: aktiver Prototyp mit verifiziertem Debug- und Digital-Twin-Pfad
- Aktive Hauptphasen: Dokumentationssynchronisierung, Security-/Safety-Vorbereitung, reale Nachweisfuehrung
- Entwicklungsrealitaet: Controller → Receiver + Bridge per ESP-NOW, Receiver → Arduino per I2C, Bridge → MQTT → Pi, Dashboard + ROS 2 als beobachtender Twin
- Aktuelle Greifer-Eingabe: Potentiometer auf `GPIO1` aus Robustheitsgruenden
- Naechste grosse Schwelle: reale Safety- und Security-Freigabe des Arms

## Aktuelle Phasenampel

| Phase | Status | Einordnung |
| --- | --- | --- |
| Doku / Management | in Arbeit | Root-, Bereichs- und Nachweisdoku werden auf denselben Ist-Stand gezogen |
| Toolchain / Flash | stabil | Arduino IDE 3.3.7 plus Custom-Board etabliert |
| Hardware / Prototyp | weit fortgeschritten | Arm-Prototyp steht, Controller sitzt auf Lochraster/Perfboard |
| Sensorik / Kommunikation | stabil fuer Twin | drei IMUs, Potentiometer, ESP-NOW, Bridge und I2C sind fuer Debug/Twin belastbar |
| Digital Twin | aktiv | Dashboard und ROS 2 sind gemappt und verifiziert |
| Safety / Security Produktivpfad | offen | reale Freigabe bleibt bewusst getrennt und noch nicht abgeschlossen |

## Bereits erledigt

- [x] Root-, Framework- und Bereichsdokumentation strukturiert
- [x] drei BNO055 ueber PCA9548A validiert
- [x] `ImuPaket v4` mit Notaus-Flag und Multi-Peer-ESP-NOW etabliert
- [x] Bridge-ESP32 mit WiFi/MQTT und OTA als Debug-Infrastruktur aufgebaut
- [x] Controller auf Lochraster/Perfboard ueberfuehrt
- [x] aktueller Arm-Prototyp mechanisch fertig aufgebaut
- [x] Greifer-Eingabe auf Potentiometer aus Robustheitsgruenden umgestellt
- [x] Dashboard-3D-Modell auf aktuellen Gelenkstand gemappt
- [x] ROS-2-Wandmontage, Live-MQTT, Recorder, Replay, Plot und Live-Monitor aufgebaut
- [x] Debug-Bridge Controller → ESP-NOW → Bridge → MQTT → Pi end-to-end verifiziert

## Noch offen im aktuellen Schwerpunkt

- [ ] Stock-Baseline und Learning-Mode des Originalkits als Nachweis nachziehen
- [ ] aktuelle Wiring-, Assembly- und Messdokumente fuer den realen Prototyp vervollstaendigen
- [ ] Security-Uplift mit Session-/Auth-Pfad dokumentieren und pruefen
- [ ] reale Safety-Faelle, Limits und Timeout-Verhalten formal in `tests/` nachziehen
- [ ] bestaetigten Twin-Stand kontrolliert in reale Armfreigabe ueberfuehren

## Naechste sinnvolle Arbeitspakete

1. Stock-/Learning-Mode des Originalkits dokumentieren und klar gegen den modifizierten Projektstand abgrenzen
2. aktuelle Perfboard-, Wiring- und Assembly-Nachweise vervollstaendigen
3. Security- und Safety-Faelle aus dem realen Steuerpfad formal beschreiben
4. bestaetigten Digital-Twin-Stand fuer reale Bewegungsfreigabe vorbereiten

## Management-Risiken

- Wenn Root-, Bereichs- und dated Nachweisdoku auseinanderlaufen, wird der reale Stand schnell missverstaendlich.
- Der aktuelle Twin-Stand ist belastbar fuer Debugging, aber noch keine produktive Bewegungsfreigabe.
- Stock-Herstellerstand und modifizierter Projektpfad muessen sichtbar getrennt bleiben.

## Pflegepflicht

- nach jedem groesseren Meilenstein aktualisieren
- `ROADMAP.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps zusammen pruefen
- nach jeder relevanten Aenderung `bash ./scripts/update_docs.sh` ausfuehren
