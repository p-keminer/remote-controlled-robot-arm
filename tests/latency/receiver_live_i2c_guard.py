#!/usr/bin/env python3
"""Guard-Test fuer den vorbereiteten Receiver-Live-I2C-Pfad."""

from pathlib import Path


REPO = Path(__file__).resolve().parents[2]
RECEIVER = REPO / "firmware/esp32_receiver/esp32_receiver.ino"
MAPPING = REPO / "firmware/esp32_receiver/live_i2c_mapping.h"


def extract_function(source: str, name: str) -> str:
    marker = f"void {name}("
    start = source.find(marker)
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


def require(text: str, needle: str, message: str) -> None:
    if needle not in text:
        raise AssertionError(message)


def main() -> None:
    source = RECEIVER.read_text(encoding="utf-8")
    require(source, "#define RECEIVER_MODE_TEST_SWEEP 1", "Test-Sweep muss Default bleiben")
    require(source, "#define RECEIVER_MODE_LIVE_I2C 0", "Live-I2C muss Default deaktiviert sein")
    require(source, "#error \"Nur ein Receiver-I2C-Modus darf aktiv sein.\"", "Modus-Konflikt muss Compile-Error sein")
    require(source, "LIVE_I2C_FRAME_INTERVALL_MS 20", "Live-I2C muss auf 50Hz vorbereitet sein")
    require(source, "LIVE_I2C_STALE_MS 250", "Stale-Pakete muessen nach 250ms neutralisieren")
    require(source, "LIVE_I2C_MAX_BYTE_STEP 2", "Byte-Slew-Limiter muss auf 2 vorbereitet sein")
    require(source, "LIVE_I2C_GRIPPER_MAX_BYTE_STEP 12", "Greifer muss einen eigenen schnelleren Slew-Limiter haben")

    callback = extract_function(source, "beiEmpfang")
    if "i2c_frame_senden" in callback or "test_sweep_update" in callback:
        raise AssertionError("ESP-NOW-Callback darf keine I2C-Frames senden")
    if "Serial." in callback:
        raise AssertionError("ESP-NOW-Callback darf nicht seriell loggen")
    require(callback, "letztes_gueltiges_paket", "Callback muss letztes gueltiges Paket puffern")
    require(callback, "neues_gueltiges_paket = true", "Callback muss neues gueltiges Paket markieren")

    live_fn = extract_function(source, "live_i2c_update")
    require(live_fn, "FRAME_FLAG_ESTOP", "Live-I2C muss Notaus auf ESTOP mappen")
    require(live_fn, "FRAME_FLAG_NEUTRAL", "Live-I2C muss stale Pakete auf Neutral mappen")
    require(live_fn, "live_achse_anwenden", "Live-I2C muss die achsweise Freigabe mit Slew-Limiter verwenden")
    require(source, "return slew_byte(aktuell, freigegeben ? ziel : LIVE_I2C_NEUTRAL_BYTE)",
            "Achsfreigabe muss deaktivierte Achsen neutralisieren und aktive Achsen slew-limiten")
    require(live_fn, "live_greifer_anwenden", "Live-I2C muss den Greifer separat slew-limiten")
    require(live_fn, "receiver_i2c_frame_senden", "Live-I2C muss I2C-Frames nur im Loop-Kontext senden")
    require(source, "i2c_frame_senden(flags, basis, schulter, ellbogen, handgelenk, greifer)",
            "Receiver-I2C-Wrapper muss den echten Frame-Sender kapseln")
    require(source, "RECEIVER_LIVE_ENABLE_GRIPPER", "Live-I2C muss achsweise freischaltbar sein")

    mapping = MAPPING.read_text(encoding="utf-8")
    for needle in [
        "REFERENCE_POSE",
        "basis_aus_imu",
        "schulter_aus_imu",
        "ellbogen_aus_imu",
        "handgelenk_raw_deg_aus_imu",
        "handgelenk_byte_aus_deg",
        "greifer_aus_prozent",
    ]:
        require(mapping, needle, f"Mapping-Baustein fehlt: {needle}")
    require(mapping, "LIVE_WRIST_CENTER_OFFSET_DEG", "Wrist-Mapping muss den Hardware-Neutraloffset dokumentieren")
    require(mapping, "LIVE_WRIST_WRAP_REJECT_DEG", "Wrist-Mapping muss Euler-/Wrap-Spruenge abfangen")
    require(mapping, "handgelenk_deg_stabilisieren", "Wrist-Mapping muss vor der Byte-Ausgabe stabilisiert werden")
    require(mapping, "paket->sensoren[0].roll, paket->sensoren[1].roll",
            "Hardware-Wrist muss die stabile relative Roll-Differenz nutzen")
    require(mapping, "live_wrist_zero_done", "Wrist-Mapping muss eine gesetzte Wrist-Mitte verfolgen")

    print("OK: Receiver Live-I2C ist sicher vorbereitet und standardmaessig deaktiviert")


if __name__ == "__main__":
    main()
