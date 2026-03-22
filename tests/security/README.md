# tests/security

## Zweck

Dieser Ordner ist fuer sicherheitsbezogene Test- und Nachweisfaelle reserviert, die sich von physischer `Safety` unterscheiden.

## Aktueller Stand

Die Struktur ist vorbereitet.
Konkrete Security-Faelle folgen spaeter mit der Implementierung der Kommunikations- und Servicepfade.

## Inhalt

- Abweisung unbekannter Peers
- Verhalten bei Replay-, Duplikat- oder ungueltigen Frames
- Verhalten bei unverschluesseltem Unicast oder fehlender LMK
- Verhalten bei verwundbarer oder nicht freigegebener Stack-Version
- Validierung von UART-Frames und Servicezugriffen
- Nachweise fuer Secret- und Provisioning-Disziplin

## Regeln

- Security-Testfaelle muessen klar zwischen Angriff, Fehlverhalten und erwarteter Abwehrreaktion unterscheiden
- physische Bewegungssicherheit bleibt primaer in `tests/safety/`

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../../security/`, `../../COMMUNICATION_FRAMEWORK.md` und `../../SECURITY_FRAMEWORK.md`
