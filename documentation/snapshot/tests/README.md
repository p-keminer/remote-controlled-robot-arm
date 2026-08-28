# Tests

Dieser Ordner enthaelt ausfuehrbare Regression-Guards. Der zusammengefuehrte Hardware-, Compile- und Verifikationsstand steht in [`docs/TESTING.md`](../docs/TESTING.md).

## Ausfuehrbare Guards

Vom Repository-Root:

```powershell
python -B tests/latency/controller_hotloop_guard.py
python -B tests/latency/receiver_live_i2c_guard.py
```

- [`controller_hotloop_guard.py`](latency/controller_hotloop_guard.py) schuetzt den Controller-Hotpath vor langsamen Status-/Kalibrierabfragen und prueft die erwarteten Diagnose- und NVS-Regeln.
- [`receiver_live_i2c_guard.py`](latency/receiver_live_i2c_guard.py) schuetzt den deaktivierten Live-I2C-Default, Callback-/Loop-Trennung, Stale-/Notaus-Abbildung und achsweise Freigaben.

Beide Tests analysieren Quelltextstrukturen. Sie simulieren keine Hardware und messen weder Funklatenz noch Servo- oder Security-Verhalten.

## Nachweisarten

| Art | Aussage |
| --- | --- |
| Compile | Quellstand uebersetzt mit dokumentierter Toolchain |
| Guard | erwartete statische Invarianten sind vorhanden |
| Bench | abgegrenzte Hardwarefunktion wurde real beobachtet |
| Integration | kompletter Steuer- oder Debugpfad wurde real verbunden |
| Safety | Fehlerreaktion und physische Grenzen wurden gemessen |
| Security | Angriffs- und Rechteannahmen wurden gegen die reale Konfiguration geprueft |

Compile-Befehle stehen in [`docs/BUILD.md`](../docs/BUILD.md). Safety und Security bleiben getrennt in [`SAFETY.md`](../SAFETY.md) und [`SECURITY.md`](../SECURITY.md). Ein Debug-/Twin-Erfolg ist keine Bewegungs- oder Produktivfreigabe.
