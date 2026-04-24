# Threat Model

Dieses Dokument beschreibt die wichtigsten Schutzgueter und Missbrauchspfade des aktuellen Projektstands.

## Schutzgueter

- autorisierte Bewegung ueber den Steuerpfad
- Integritaet des I2C- und Funkpfads
- Integritaet von Referenz- und Kalibrierdaten
- Schutz der lokalen Debug- und Zugangsdaten

## Wichtige Missbrauchspfade

- unautorisierte Beeinflussung des Steuerpfads
- unklare Trennung zwischen Debug- und Produktivpfad
- Leckage lokaler Zugangsdaten
- implizite Freigabe durch erfolgreiches Dashboard-/ROS-Debugging

## Aktuelle Gegenmassnahme

Die zentrale Projektregel ist derzeit die klare Trennung:

- produktive Bewegung ueber lokalen Steuerpfad
- Beobachtung ueber Debugpfad

Weitere Haertung bleibt offen.
