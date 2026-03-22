# MEMORY.md

Dieses Dokument enthaelt den akkumulierten Arbeits- und Entscheidungskontext des Projekts.
Es ist verbindlich fuer alle KI-Einheiten (Claude, Codex, andere Agenten) und ergaenzt `AGENTS.md` sowie `CLAUDE.md`.
Beim Sitzungsstart oder nach Kontextkomprimierung ist dieses Dokument zu lesen.

---

## Projektstand (Stand 2026-03-22)

- Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 + boards.local.txt fuer N16R8, PlatformIO als Fallback
- ESP-NOW Unicast laeuft: Controller → Receiver per MAC, ImuPaket v1 mit XOR-Pruefsumme und Frame-Zaehler
- BNO055 validiert: I2C-Scan (0x29, ADR=3V3), Rohwerte stabil, Gyro-Kalibrierung 3/3
- PCA9548A-Mux validiert: Kanal 0 und 1 einzeln umschaltbar, zwei BNO055 gleichzeitig ausgelesen
- Flex-Sensor validiert: GPIO1 ADC1, Spannungsteiler 10kOhm Pull-Down, gerade=1108, maximal gebogen=940, Bereich 168 Counts
- MACs dokumentiert in `security/local/device_identities.local.txt` (gitignoriert)
- Naechste Schritte: LED-Debugging (GPIO4/5/6/7/10), Buzzer GPIO21, Roboterarm aufbauen, UART zu Arduino

---

## Hardware-Fakten (bestaetigt)

- Modul: `ESP32-S3-WROOM-1-N16R8` (16 MB Flash, 8 MB Octal PSRAM)
- Boardrevision: v1.0 — onboard RGB-LED auf **GPIO48** (nicht GPIO38)
- GPIO35, GPIO36, GPIO37: intern durch Octal-PSRAM belegt — **nicht fuer User-I/O nutzbar**
- Anzahl: 4 Boards (2 aktiv, 2 Reserve)
- BNO055: GY-BNO055 Clone (Senzooe, Amazon B0D2J5PY29) — kein Adafruit-Breakout
- I2C-Mux: PCA9548A (ca. 3–4 Module)
- Flex-Sensor: vorhanden, auf GPIO1 getestet

---

## Verbindliche Feedback-Regeln

### Niemals loeschen ohne Rueckfrage

Dateien und Ordner **niemals** loeschen ohne vorher den Nutzer zu fragen — auch wenn der Ordner vom Assistenten angelegt wurde.

**Warum:** `rm -rf` auf Windows-Pfaden via WSL umgeht den Papierkorb. Verzeichnisse koennen Nutzerinhalte enthalten.

**Anwendung:** Vor jedem `rm`, `rmdir` oder vergleichbaren Loeschoperationen immer den Nutzer fragen.

---

### Code auf Deutsch verfassen

Variablennamen, Struct-Felder, Enums, Konstanten und Kommentare auf **Deutsch** verfassen.

**Warum:** Projektentscheidung — das Projekt soll sprachlich konsistent auf Deutsch gehalten werden, auch im Code.

**Anwendung:**
- Neue Variablen, Structs, Enums, Konstanten, Kommentare auf Deutsch
- Funktionsnamen deutsch oder englisch, aber konsistent innerhalb eines Moduls
- Arduino/ESP-IDF API-Bezeichner und externe Bibliothekstypen unveraendert lassen
- Bestehenden Code nicht rueckwirkend umbenennen — nur neue und geaenderte Dateien

---

### Flashen ueber Arduino IDE auf Windows

Firmware immer als `.ino`-Sketch in Arduino IDE auf Windows flashen. PlatformIO nur als Notfall-Fallback.

**Warum:** Arduino IDE reicht fuer dieses Projekt vollstaendig aus. PlatformIO ist als Lernziel nebenbei, nicht als Hauptweg.

**Anwendung:**
- Sketches als `.ino` bereitstellen — kein `#include <Arduino.h>` noetig in `.ino`-Dateien
- Arduino IDE oeffnet `.ino`-Dateien direkt aus WSL via `\\wsl$\Ubuntu\...` Pfad
- Lokale Header (`peer_config.local.h`) liegen im selben WSL-Verzeichnis wie die `.ino` und werden automatisch gefunden
- Upload-Anweisungen immer zuerst fuer Arduino IDE beschreiben

---

### Kein Hartcodieren in Firmware

MACs, Schluessel, Peer-Adressen und Geraete-IDs niemals direkt in `.ino`- oder `.cpp`-Dateien schreiben.

**Warum:** Das Projekt ist oeffentlich auf GitHub. Hartcodierte MACs und IDs waeren sichtbar und verstossen gegen die Sicherheitsregeln.

**Anwendung:**
- Immer `#include "peer_config.local.h"` verwenden statt Rohwerte einzutragen
- Committed werden nur `peer_config.template.h` mit Platzhaltern
- `.gitignore` deckt `firmware/**/peer_config.local.*` ab

---

### Vor jedem Push: Drei Pflichtpruefungen

Vor jedem `git push` sind folgende Pruefungen Pflicht:

1. **Secret-Pruefung**: Keine Secrets, MACs, Schluessel, IPs oder Passwoerter in getrackten Dateien ausserhalb von `.gitignore`.
2. **Dokumenten-Konsistenz**: Alle `[x]`-Punkte muessen ueber `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps konsistent und widerspruchsfrei sein.
3. **README-Aktualitaet**: `README.md`-Abschnitte `Aktueller Fokus` und `Aktueller Entwicklungsstand` muessen den echten Projektstand widerspiegeln.

Danach `bash ./scripts/update_docs.sh` ausfuehren.

**Warum:** Dokumente liefen in der Vergangenheit auseinander — ROADMAP.md zeigte Phase 6 als offen, obwohl BNO055, Mux, Flex-Sensor und ESP-NOW laengst validiert waren.
