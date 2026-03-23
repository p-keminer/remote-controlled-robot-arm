# Safety Notice

Dieses Dokument ist **kein** Teil der Apache-2.0-Lizenz und schraenkt deren Rechte nicht ein.
Es beschreibt den sicherheitsbezogenen Projektkontext fuer dieses Robotiksystem.

## Zweck

- sichtbarer Sicherheitshinweis fuer Projektmitarbeitende und spaetere Nutzende
- klare Trennung zwischen Open-Source-Lizenz und sicherheitsbezogener Projektverantwortung
- Hinweis, dass Freigabe des Quellcodes keine Freigabe fuer sicherheitskritischen Betrieb bedeutet

## Wichtige Hinweise

- Dieses Projekt kann physische Hardware bewegen und spaeter auf weitere Robotik- oder Aktuatorsysteme uebertragbar sein.
- Das Projekt ist **nicht** sicherheitszertifiziert.
- Es ist nicht fuer medizinische, industrielle, autonome oder sonstige sicherheitskritische Einsatzumgebungen freigegeben.
- Der aktuelle Stand ist ein Entwicklungs-, Test- und Forschungsprojekt und keine produktionsreife Sicherheitsloesung.
- Reale Inbetriebnahme, Stromversorgung, Aufbau, Montage, Verdrahtung, Grenzwerte, Watchdog-Verhalten und Not-Stop-Logik muessen vor echtem Betrieb separat validiert werden.
- Die projektspezifischen Safety-Regeln stehen zusaetzlich in `SAFETY_FRAMEWORK.md`.

## Verbindliche Projekteinordnung

- Die Apache-2.0-Lizenz erlaubt Nutzung, Veraenderung und Weitergabe der projekt-eigenen Inhalte.
- Diese Freigabe bedeutet **nicht**, dass das System technisch oder rechtlich fuer reale Sicherheitsanwendungen freigegeben ist.
- Wer das Projekt aufbaut, aendert, portiert oder in Betrieb nimmt, traegt die Verantwortung fuer die sichere Umsetzung im konkreten Einsatz.
