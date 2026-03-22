# security/local

## Zweck

Dieser Ordner ist ausschliesslich fuer lokale, nicht-repotaugliche Sicherheits- und Geraetedaten reserviert.

## Inhalt

Hier gehoeren spaeter nur lokale Dateien hinein wie:

- Pairing- oder Peer-Daten
- lokale Geraeteidentitaeten
- Test- oder Provisioning-Schluessel
- lokale Stack- und Freigabenotizen
- lokale Service- oder Freigabenotizen

Typische lokale Dateinamen koennen sein:

- `device_identities.local.txt`
- `stack_versions.local.md`
- `app_auth_keys.local.env`

## Regeln

- Inhalte dieses Ordners duerfen nicht nach Git eingecheckt werden
- echte Secrets, Schluessel und lokale Identitaetsdaten gehoeren nur in die dafuer vorgesehenen lokalen Dateien
- Beispiel- oder Strukturwissen fuer diese Daten gehoert in die oeffentliche Security-Dokumentation, nicht in die lokalen Echtdateien

## Hinweis

Die echten lokalen Dateien werden ueber `.gitignore` blockiert und nicht in den Dokumentationssnapshot uebernommen.
