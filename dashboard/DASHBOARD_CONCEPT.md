# Dashboard-Konzept

Diese kleine Kompatibilitaetsseite bleibt erhalten, weil eine vorhandene
Konfigurationsvorlage darauf verweist. Betrieb, Topics, Twin-Mapping und
bekannte Grenzen stehen im [`dashboard/README.md`](README.md). Der Debugpfad
ist separat vom Bewegungsweg und bleibt ein aktiver Teil des Repositories.

## OTA-Sicherheitskonzept

`mcp/ota_config.template.py` ist eine Vorlage; der aktuelle
`mqtt_mcp_server.py` laedt sie nicht. Eine spaetere OTA-Aktivierung waere ein
sicherheitskritischer Schreibpfad und braucht vor jeder Implementierung:

- ein eigenes, starkes OTA-Geheimnis statt Platzhaltern oder gemeinsam
  genutzten WiFi-/MQTT-Passwoertern;
- lokale, gitignorierte Konfiguration mit restriktiven Dateirechten;
- Netzwerksegmentierung und eine explizite Ziel- und Firmwarepruefung;
- einen eng begrenzten Broker-/Dienstaccount statt Schreibrecht auf
  `robotarm/#`;
- nachvollziehbares Logging sowie eine getestete Recovery-Moeglichkeit;
- eine physisch sichere Armkonfiguration mit getrenntem Servostrom waehrend
  Flash und Neustart.

Der MCP-Server ist wegen seines vorhandenen `mqtt_publish`-Werkzeugs nicht
intrinsisch read-only. Die mitgelieferte Broker-ACL muss Schreibversuche des
Reader-Kontos wirksam blockieren. Weitere Regeln stehen in
[`SECURITY.md`](../SECURITY.md).
