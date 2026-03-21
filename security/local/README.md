# security/local

## Zweck

Dieser Ordner ist ausschliesslich fuer lokale, nicht-repotaugliche Sicherheits- und Geraetedaten reserviert.

## Inhalt

Hier gehoeren spaeter nur lokale Dateien hinein wie:

- Pairing- oder Peer-Daten
- lokale Geraeteidentitaeten
- Test- oder Provisioning-Schluessel
- lokale Service- oder Freigabenotizen

## Regeln

- Inhalte dieses Ordners duerfen nicht nach Git eingecheckt werden
- echte Secrets, Schluessel und lokale Identitaetsdaten gehoeren nur in die dafuer vorgesehenen lokalen Dateien
- Beispiel- oder Strukturwissen fuer diese Daten gehoert in die oeffentliche Security-Dokumentation, nicht in die lokalen Echtdateien

## Hinweis

Die echten lokalen Dateien werden ueber `.gitignore` blockiert und nicht in den Dokumentationssnapshot uebernommen.
