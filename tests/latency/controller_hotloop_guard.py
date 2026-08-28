#!/usr/bin/env python3
"""Guard-Test fuer den ESP32-Controller-Hotpath.

Der schnelle Bewegungsloop darf keine langsamen BNO055-Status- oder
Kalibrierabfragen enthalten. Diese Abfragen muessen gestaffelt ausserhalb des
Hotpaths laufen, sonst entstehen periodische Paketpausen und sichtbares Ruckeln.
"""

from pathlib import Path
import re


REPO = Path(__file__).resolve().parents[2]
SKETCH = REPO / "firmware/esp32_controller/esp32_controller.ino"


def extract_function(source: str, name: str) -> str:
    start = -1
    for return_type in ["void", "bool", "float", "uint8_t", "uint16_t", "uint32_t", "int"]:
        marker = f"{return_type} {name}("
        start = source.find(marker)
        if start >= 0:
            break
    if start < 0:
        raise AssertionError(f"Funktion {name} nicht gefunden")

    brace_start = source.find("{", start)
    if brace_start < 0:
        raise AssertionError(f"Funktionsrumpf {name} nicht gefunden")

    depth = 0
    for pos in range(brace_start, len(source)):
        char = source[pos]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[brace_start : pos + 1]

    raise AssertionError(f"Funktionsrumpf {name} ist nicht geschlossen")


def main() -> None:
    source = SKETCH.read_text(encoding="utf-8")
    loop_normal = extract_function(source, "loop_normal")

    forbidden_hotpath_calls = [
        "getSystemStatus",
        "getCalibration",
        "kalib_speichern",
    ]
    for call in forbidden_hotpath_calls:
        if call in loop_normal:
            raise AssertionError(f"{call} liegt noch im schnellen loop_normal()")

    if "paket.kalib[i] = letzter_kalib_status[i];" not in loop_normal:
        raise AssertionError("loop_normal() sendet nicht den gecachten Kalibrierstatus")

    slow_function = extract_function(source, "controller_status_langsam_pruefen")
    for call in ["getSystemStatus", "kalib_speichern"]:
        if call not in slow_function:
            raise AssertionError(f"{call} fehlt in der langsamen Statuspruefung")
    if "kalib_status_lesen" not in slow_function:
        raise AssertionError("Langsame Statuspruefung muss Kalibrierstatus ueber Helper lesen")
    kalib_status_live_fn = extract_function(source, "kalib_status_live_lesen")
    if "getCalibration" not in kalib_status_live_fn:
        raise AssertionError("kalib_status_live_lesen() muss getCalibration kapseln")
    kalib_laden_fn = extract_function(source, "kalib_laden")
    if "letzter_kalib_status[idx] = {3, 3, 3, 3};" not in kalib_laden_fn:
        raise AssertionError("Geladene NVS-Offets muessen im Livepfad effektiv kalibriert melden")

    if "STATUS_CHECK_INTERVALL" not in source:
        raise AssertionError("STATUS_CHECK_INTERVALL fehlt")

    if "KALIB_SAVE_STABLE_COUNT 3" not in source:
        raise AssertionError("Kalibrierung muss vor dem Speichern mehrfach stabil sein")

    if "kalib_todo_ausgeben" not in source:
        raise AssertionError("Kalibrierausgabe muss konkrete TODO-Hinweise enthalten")

    if "static bool debug_status_aktiv = false;" not in source:
        raise AssertionError("Periodischer Serial-Status muss im Livebetrieb standardmaessig aus sein")
    for command in ['"TIMING"', '"DEBUGON"', '"DEBUGOFF"']:
        if command not in source:
            raise AssertionError(f"Serial-Debug-Befehl fehlt: {command}")

    if "static bool status_diag_aktiv = false;" not in source:
        raise AssertionError("Langsame Hintergrunddiagnose muss im Livebetrieb standardmaessig aus sein")
    for command in ['"DIAGON"', '"DIAGOFF"']:
        if command not in source:
            raise AssertionError(f"Diagnose-Befehl fehlt: {command}")

    cal_branch = re.search(
        r'} else if \(eingabe == "CAL0".*?} else if \(eingabe == "STOP"\)',
        source,
        re.DOTALL,
    )
    if not cal_branch:
        raise AssertionError("CAL0/CAL1/CAL2-Zweig nicht gefunden")
    if "prefs.remove" in cal_branch.group(0):
        raise AssertionError("CAL0/CAL1/CAL2 darf alte Offsets nicht sofort loeschen")

    print("OK: Controller-Hotpath ist von langsamen Status-/Kalibrierabfragen getrennt")


if __name__ == "__main__":
    main()
