# Provisioning and Debug Rules

Dieses Dokument beschreibt den Umgang mit lokalen Secrets, Pairingdaten und Debugkonfiguration.

## Regeln

- echte Zugangsdaten gehoeren nie ins Repository
- lokale Konfigurationen liegen in gitignorierten Dateien
- MACs, MQTT-Credentials, WiFi-Daten und lokale Schluessel werden getrennt vom Code gepflegt
- Debugpfad und Produktivpfad teilen sich keine stillschweigend freigegebenen Echtwerte

## Verwandte lokale Dateien

- `security/local/`
- firmware-nahe `*.local.*`-Dateien
- lokale ROS-/MQTT-Konfiguration
