// peer_config.template.h — Vorlage fuer die Bridge-ESP32 Peer-Konfiguration
// Kopiere diese Datei als peer_config.local.h und ersetze die Platzhalter-MAC.
// peer_config.local.h ist per .gitignore geblockt und darf NICHT eingecheckt werden.

// MAC-Adresse des Controllers (fuer Absendervalidierung)
static uint8_t controller_adresse[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
