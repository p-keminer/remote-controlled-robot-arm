# tests/integration

## Zweck

Dieser Ordner ist fuer Ende-zu-Ende-Tests ueber mehrere Teilbereiche reserviert.

## Aktueller Stand

Die Struktur steht, und ein erster Teilpfad Controller -> Receiver ist ueber `ESP-NOW` bench-validiert.
Die eigentliche Gesamtkette bis zum Arduino und zu den Servos ist noch offen.

## Inhalt

- Controller -> Receiver -> Arduino Testablaeufe
- Kalibrierung plus reale Bewegungsuebertragung
- Wiederanlauf- und Verbindungsabbruchszenarien

## Regeln

- Integrationsfaelle sollen Eingangsbedingung, Ablauf und erwartetes Verhalten explizit beschreiben
- offene Restunsicherheiten direkt am Testfall sichtbar machen

## Schnittstellen/Abhaengigkeiten

- bezieht Anforderungen aus den Framework-Dokumenten und reale Nachweise aus `docs/`
