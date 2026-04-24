Du pruefst JEDES Dokument im Repository gegen den tatsaechlichen Code und behebst alle Inkonsistenzen.

## Ablauf

### Schritt 1: Code-Fakten extrahieren

Lies die drei Firmware-Sketches und extrahiere die harten Fakten:

**Aus `firmware/esp32_controller/esp32_controller.ino`:**
- PROTOKOLL_VERSION, sizeof(ImuPaket), ImuPaket-Struct-Felder
- Alle GPIO-Pin-Defines (LEDs, Notaus, Flex, I2C, Mux)
- LED-Verhalten (welcher Pin, wann an/aus, RGB-Farben)
- Notaus-Logik (Toggle/NC, Pin, Entprellung, Pull-Up)
- WiFi-Kanal (`esp_wifi_set_channel`)
- Anzahl Sensoren, Schwellwerte
- ESP-NOW Peers (Receiver + Bridge)

**Aus `firmware/esp32_receiver/esp32_receiver.ino`:**
- PROTOKOLL_VERSION, Paketgroesse-Pruefung
- GPIO-Pins (LEDs, RGB)
- Notaus-Flag-Auswertung
- WiFi-Kanal
- Timeout-Werte

**Aus `firmware/esp32_bridge/esp32_bridge.ino`:**
- PROTOKOLL_VERSION, Paketgroesse-Pruefung
- GPIO-Pins (LEDs, RGB)
- MQTT-Topics und JSON-Felder
- Notaus-Flag-Auswertung und MQTT-Weiterleitung
- WiFi-Kanal, OTA-Konfiguration

### Schritt 2: Alle Dokumente scannen

Finde ALLE `.md`-Dateien im Repo (ausser `documentation/snapshot/` und `official_downloads/`).

Pruefe JEDE Datei gegen die extrahierten Code-Fakten auf:

- **Pin-Zuordnungen**: GPIO-Nummern, Pin-Funktionen, Pull-Up/Down
- **Protokollversion**: ImuPaket vX, Paketgroesse in Bytes
- **Notaus-Beschreibung**: Toggle vs NC, GPIO-Pin, Logik, Entprellung
- **LED-Schema**: "aus=OK, an=Problem" vs "blinken=Problem", RGB-Farben (orange/rot/aus)
- **WiFi-Kanal**: Kanal-Nummer muss mit Code uebereinstimmen
- **MQTT-Topics und JSON-Felder**: Feldnamen, Datentypen
- **Struct-Felder**: Reihenfolge, Typen, Bitfeld-Definitionen
- **Schwellwerte**: Kalibrierung, Timeouts, Intervalle
- **Dateinamen und Pfade**: Existieren referenzierte Dateien noch?
- **Abgeschlossene Aufgaben**: Stimmen `[x]`-Eintraege in Roadmaps und Fortschritt ueberein?

### Schritt 3: Inkonsistenzen beheben

Fuer JEDE gefundene Inkonsistenz:
1. Zeige: Datei, Zeile, IST-Wert (im Dokument), SOLL-Wert (aus Code)
2. Korrigiere das Dokument (NICHT den Code!)
3. Bei Unsicherheit: markiere als "MANUELL PRUEFEN" und frage nach

### Schritt 4: Snapshot und update_docs.sh

Nach allen Korrekturen:
1. Fuehre `bash ./scripts/update_docs.sh` im Projektverzeichnis aus
2. Zeige eine Zusammenfassung:
   - Anzahl gepruefter Dokumente
   - Anzahl gefundener Inkonsistenzen
   - Anzahl behobener Inkonsistenzen
   - Liste offener Punkte (falls vorhanden)

## Regeln

- NUR Dokumente aendern, NIEMALS Code
- Historische Eintraege (z.B. "ImuPaket v3 eingefuehrt am 2026-03-26") NICHT aendern — die beschreiben was damals passiert ist
- `documentation/snapshot/` wird durch update_docs.sh automatisch neu erzeugt — nicht manuell bearbeiten
- `official_downloads/` niemals anfassen
- Ton und Struktur der Dokumente beibehalten
- Deutsche Umlaute als ae/oe/ue/ss schreiben (ASCII-kompatibel)
- Alle Aenderungen muessen nachvollziehbar sein
