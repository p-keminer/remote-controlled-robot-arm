# HELP.md

Schnellreferenz fuer haeufig benoetigte Befehle im Projektalltag.

## USB-Board zwischen Windows und WSL wechseln

Das Board kann immer nur auf einer Seite aktiv sein.

**An WSL geben** (fuer PlatformIO im Terminal):
```bash
"/mnt/c/Program Files/usbipd-win/usbipd.exe" attach --wsl --busid 2-2
```

**An Windows zurueckgeben** (fuer Arduino IDE):
```bash
"/mnt/c/Program Files/usbipd-win/usbipd.exe" detach --busid 2-2
```

**Aktuellen Status pruefen:**
```bash
"/mnt/c/Program Files/usbipd-win/usbipd.exe" list
```

> Hinweis: BUSID `2-2` gilt fuer den aktuellen USB-Anschluss. Falls das Board an einem anderen Port eingesteckt wird, kann sich die BUSID aendern — dann zuvor `list` ausfuehren.

---

## PlatformIO (WSL)

**Board flashen:**
```bash
export PATH="$HOME/.platformio/penv/bin:$PATH"
cd ~/projects/embedded/robotic-arm/firmware/esp32_controller
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
bash ~/projects/embedded/robotic-arm/scripts/sync_to_windows.sh
```

**Dauerhafter Watcher** (synct automatisch bei jeder Aenderung):
```bash
bash ~/projects/embedded/robotic-arm/scripts/watch_and_sync.sh
```

Kopiert `.ino` und `.h` aus `firmware/` nach:
`C:\Users\pkemi\OneDrive\Desktop\Projekte\5doof_robotarm\firmware\`

---

## Dokumentation aktualisieren

Nach jeder groesseren Aenderung:
```bash
bash ~/projects/embedded/robotic-arm/scripts/update_docs.sh
```

---

## Arduino IDE (Windows) — Boardeinstellungen ESP32-S3-WROOM-1-N16R8

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
| Port | COM9 |
