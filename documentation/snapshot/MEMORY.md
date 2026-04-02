# MEMORY.md

Dieses Dokument enthaelt den akkumulierten Arbeits- und Entscheidungskontext des Projekts.
Es ist verbindlich fuer alle KI-Einheiten (Claude, Codex, andere Agenten) und ergaenzt `AGENTS.md` sowie `CLAUDE.md`.
Beim Sitzungsstart oder nach Kontextkomprimierung ist dieses Dokument zu lesen.

---

## Projektstand (Stand 2026-03-26)

- Toolchain vollstaendig eingerichtet: Arduino IDE 3.3.7 + boards.local.txt als Hauptumgebung, PlatformIO als lokaler Fallback und Gegencheck
- ESP-NOW Unicast laeuft: Controller → Receiver per MAC, ImuPaket v4 mit drei IMUs, Kalibrierungsstatus und NVS-Persistenz (bestaetigt 2026-03-26)
- BNO055 validiert: drei Sensoren (Oberarm/Unterarm/Hand) ueber PCA9548A-Mux Kanaele 0/1/2, Kalibrierungsoffsets persistent im NVS
- Einzelkalibrierungsmodus per Serial: CAL0/CAL1/CAL2, RECAL, STOP
- Flex-Sensor validiert: GPIO1 ADC1, Spannungsteiler 10kOhm Pull-Down, gerade=1108, maximal gebogen=940, Live-Plausibilitaetspruefung (200-3800)
- LED-Debugging bench-validiert: Controller Ampelsystem (Gruen Hand, Gelb Unterarm, Rot Oberarm, Blau COMMS, Weiss FAULT), Receiver (Gruen LINK, Blau UART, Gelb FAULT), alle 100 Ohm
- Live-Sensorausfallerkennung fuer IMUs und Flex-Sensor mit automatischer Wiederherstellung
- Adeept 5-DOF Roboterarm mechanisch im Stock-Zustand aufgebaut (bestaetigt 2026-03-24, Fotos unter docs/photos/)
- MACs dokumentiert in `security/local/device_identities.local.txt` (gitignoriert)
- Akku-/Lade-Arbeitsstand festgelegt: `4x Molicel INR-18650-M35A` ohne Loetfahne + `1x XTAR VC4SL`
- Stock-Funktionsentscheidung: OLED uebernehmen, Potentiometer als Fallback, Learning/Action Memory uebernehmen, Processing vorerst als Referenz-/Fallbackpfad behalten
- Naechste Schritte: dritten IMU anbinden, LED-Debugging (GPIO4/5/6/7/10), Buzzer GPIO21, Roboterarm aufbauen, UART zu Arduino, Security-Uplift erst danach

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

### Primarer Entwicklungsweg: Arduino IDE, PlatformIO als Fallback

Firmware wird primaer ueber Arduino IDE auf Windows entwickelt und geflasht. PlatformIO bleibt als lokaler Fallback, Build-Gegencheck und zusaetzlicher Referenzpfad nutzbar.

**Warum:** Arduino IDE ist der reale Hauptarbeitsweg dieses Projekts. `platformio.ini` bleibt trotzdem wertvoll als versionierte Zusatzreferenz fuer Board-, Flash- und PSRAM-Konfiguration.

**Anwendung:**
- Sketches als `.ino` bereitstellen — kein `#include <Arduino.h>` noetig in `.ino`-Dateien
- Arduino IDE oeffnet `.ino`-Dateien bei Bedarf direkt aus WSL via `\\wsl$\Ubuntu\...` Pfad
- Lokale Header (`peer_config.local.h`) liegen im selben WSL-Verzeichnis wie die `.ino` und werden automatisch gefunden
- `platformio.ini` bleibt als versionierte Zusatzreferenz fuer Board- und Build-Einstellungen erhalten
- Upload-Anweisungen muessen klar kenntlich machen, ob Arduino IDE oder ausnahmsweise PlatformIO gerade der benutzte lokale Pfad ist

---

### Kein Hartcodieren — gilt fuer Firmware UND Skripte

MACs, Schluessel, Peer-Adressen und Geraete-IDs niemals direkt in `.ino`- oder `.cpp`-Dateien schreiben. Ebenso duerfen Skripte keine nutzer- oder systemspezifischen absoluten Pfade enthalten.

**Warum:** Das Projekt ist oeffentlich auf GitHub. Hartcodierte MACs, IDs und lokale Pfade (z.B. `/home/username/`, `/mnt/c/Users/username/`) sind sichtbar, verstoessen gegen Sicherheitsregeln und machen das Projekt nicht uebertragbar.

**Anwendung:**
- Immer `#include "peer_config.local.h"` verwenden statt Rohwerte einzutragen
- Committed werden nur `peer_config.template.h` mit Platzhaltern
- `.gitignore` deckt `firmware/**/peer_config.local.*` ab
- Konfigurierbare Pfade in Skripten gehoeren in gitignorierte `*.local.sh`-Dateien
- Committed werden nur Templates (z.B. `sync_config.local.sh.template`) mit generischen Platzhaltern — kein Betriebssystem, kein Benutzername, keine Verzeichnisstruktur sichtbar
- `.gitignore` deckt `scripts/*.local.sh` ab

---

### Vor jedem Push: Drei Pflichtpruefungen

Vor jedem `git push` sind folgende Pruefungen Pflicht:

1. **Secret- und Pfad-Pruefung**: Keine Secrets, MACs, Schluessel, IPs, API-Schluessel, absolute lokale Pfade (Linux, WSL oder Windows — egal) oder Passwoerter in getrackten Dateien. Gilt ausdruecklich auch fuer Skripte, Konfigurationsdateien und Kommentare.
2. **Dokumenten-Konsistenz**: Alle `[x]`-Punkte muessen ueber `ROADMAP.md`, `PROJEKT_FORTSCHRITT.md`, `PROJEKT_ABLAUFPLAN.md` und lokale Roadmaps konsistent und widerspruchsfrei sein.
3. **README-Aktualitaet**: `README.md`-Abschnitte `Aktueller Fokus` und `Aktueller Entwicklungsstand` muessen den echten Projektstand widerspiegeln.

Danach `bash ./scripts/update_docs.sh` ausfuehren.

**Warum:** Lokale Pfade in Sync-Skripten sind auf GitHub gelandet. Dokumente liefen in der Vergangenheit auseinander.
