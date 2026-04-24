# security

## Zweck

Dieser Ordner beschreibt die Sicherheits- und Debugabgrenzung des Projekts.

## Aktueller Stand

Die wichtigste aktuelle Sicherheitsgrenze lautet:

- Steuerpfad: `Controller -> Receiver -> Arduino -> Servos`
- Debugpfad: `Controller -> Bridge -> MQTT -> Pi -> Dashboard/ROS`

Der Debugpfad ist aktiv und verifiziert, bleibt aber ausdruecklich ausserhalb jeder Produktivfreigabe.
Der eigentliche Security-Uplift fuer den Produktivpfad ist noch offen.

## Inhalt

- `COMMUNICATION_SECURITY.md` fuer Funk-, I2C- und Pfadgrenzen
- `INTERFACE_SECURITY_PLAN.md` fuer Service- und Debugschnittstellen
- `PROVISIONING_AND_DEBUG_RULES.md` fuer Secrets und lokale Konfiguration
- `THREAT_MODEL.md` fuer Angreifer und Missbrauchspfade
- `local/` fuer gitignorierte Echtwerte

## Regeln

- WiFi/MQTT wird als Debugpfad behandelt, nicht als Bewegungsfreigabe.
- lokale Echtwerte gehoeren nur in `security/local/`
- Security und Safety bleiben getrennt

## Schnittstellen/Abhaengigkeiten

- `../COMMUNICATION_FRAMEWORK.md`
- `../tests/security/`
- `../tests/safety/`
