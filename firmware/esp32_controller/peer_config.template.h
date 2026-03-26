// Peer-Konfiguration Template
// Diese Datei wird committet. Fuer lokale Nutzung als peer_config.local.h kopieren
// und Platzhalter durch echte Werte ersetzen.
//
// Bezugsquelle fuer echte MACs: security/local/device_identities.local.txt

// MAC-Adresse des Receivers (Beispiel-Platzhalter)
static uint8_t empfaenger_adresse[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// Debug-Bridge MAC (optional — BRIDGE_AKTIV auf 0 zum Deaktivieren)
// Die Bridge empfaengt nur, sie kann keine Daten in den Steuerpfad einschleusen.
#define BRIDGE_AKTIV 0
static uint8_t bridge_adresse[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
