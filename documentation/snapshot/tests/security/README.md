# tests/security

## Zweck

Dieser Ordner ist fuer sicherheitsbezogene Test- und Nachweisfaelle reserviert, die sich von physischer `Safety` unterscheiden.

## Aktueller Stand

Die Struktur und die Security-Baseline sind dokumentiert.
Konkrete formale Testfaelle muessen noch aus dem aktuellen Bench-Pfad und dem geplanten Security-Uplift abgeleitet werden.

## Inhalt

- Abweisung unbekannter Peers
- Verhalten bei Replay-, Duplikat- oder ungueltigen Frames
- Verhalten bei unverschluesseltem Unicast oder fehlender LMK
- Verhalten bei Default-PMK oder fehlender expliziter PMK
- Verhalten bei alter `session_id` oder ruecklaeufigem Zaehler
- Verhalten bei ungueltigem oder fehlendem `auth_tag64`
- Verhalten bei verwundbarer oder nicht freigegebener Stack-Version
- Validierung von UART-Frames und Servicezugriffen
- Nachweise fuer Secret- und Provisioning-Disziplin

## Regeln

- Security-Testfaelle muessen klar zwischen Angriff, Fehlverhalten und erwarteter Abwehrreaktion unterscheiden
- jeder Testfall muss den verwendeten Stack-Stand und die Freigabebasis mit angeben
- physische Bewegungssicherheit bleibt primaer in `tests/safety/`

## Schnittstellen/Abhaengigkeiten

- arbeitet mit `../../security/`, `../../COMMUNICATION_FRAMEWORK.md` und `../../SECURITY_FRAMEWORK.md`
