# HELP.md

Schnellreferenz fuer haeufig benoetigte Befehle im Projektalltag.

## Grundsatz

- Arduino IDE ist die Hauptumgebung fuer Uploads und Alltagsentwicklung
- PlatformIO bleibt als lokaler Fallback und Gegencheck fuer Build und Konfiguration
- konkrete lokale Werte wie USB-BUSID, COM-Port oder Windows-Zielpfad gehoeren nicht fest in dieses Dokument, sondern in eigene lokale Notizen oder gitignorierte Konfigurationen

## USB-Board zwischen Windows und WSL wechseln

Das Board kann immer nur auf einer Seite aktiv sein.

**An WSL geben** (fuer optionale PlatformIO-Laeufe im Terminal):
```bash
usbipd.exe attach --wsl --busid <DEINE-BUSID>
```

**An Windows zurueckgeben** (fuer Arduino IDE):
```bash
usbipd.exe detach --busid <DEINE-BUSID>
```

**Aktuellen Status pruefen:**
```bash
usbipd.exe list
```

> Hinweis: Die konkrete BUSID ist lokal und kann sich mit dem USB-Port aendern. Vor jedem Attach/Detach zuerst `usbipd.exe list` ausfuehren.

---

## Arduino IDE (Windows)

Diese Einstellungen sind der Hauptpfad fuer den Projektalltag.

| Einstellung | Wert |
| --- | --- |
| Board | ESP32S3 Dev Module |
| CPU Frequency | 240MHz (WiFi) |
| Flash Mode | QIO 80MHz |
| Flash Size | 16MB (128Mb) |
| PSRAM | OPI PSRAM |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | Enabled |
| Upload Speed | 921600 |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| Port | `<DEIN-COM-PORT>` |

## PlatformIO (WSL oder Windows-Fallback)

**Board flashen:**
```bash
export PATH="$HOME/.platformio/penv/bin:$PATH"
cd /PFAD/ZUM/REPO/firmware/esp32_controller
platformio run --target upload --upload-port /dev/ttyACM0
```

**Nur kompilieren (kein Flash):**
```bash
platformio run
```

**Seriellen Output lesen (Python, da kein interaktives Terminal):**
```bash
python3 - <<'EOF'
import serial, time
s = serial.Serial('/dev/ttyACM0', 115200, timeout=4)
s.setDTR(False); s.setRTS(True); time.sleep(0.1); s.setRTS(False)
time.sleep(3)
print(s.read(4096).decode('utf-8', errors='replace'))
s.close()
EOF
```

---

## Sketches nach Windows synchronisieren

**Einmaliger Sync** (nach Aenderungen):
```bash
bash /PFAD/ZUM/REPO/scripts/sync_to_windows.sh
```

**Dauerhafter Watcher** (synct automatisch bei jeder Aenderung):
```bash
bash /PFAD/ZUM/REPO/scripts/watch_and_sync.sh
```

Zielpfad und lokale Spiegelordner werden ausschliesslich in `scripts/sync_config.local.sh` gesetzt.

---

## Dokumentation aktualisieren

Nach jeder groesseren Aenderung:
```bash
bash /PFAD/ZUM/REPO/scripts/update_docs.sh
```
