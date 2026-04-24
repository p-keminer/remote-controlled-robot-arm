# Repo-Doku-Sync Audit Checkliste

## Kontext

- Datum: 2026-04-24
- Ziel: repo-weite Dokumentationssynchronisierung auf den aktuellen Prototyp-, Potentiometer- und Twin-Stand
- Leitformulierung: Potentiometer aus Robustheitsgruenden, Controller auf Lochraster/Perfboard, Arm-Prototyp fertig, Digital Twin in Dashboard und ROS gemappt, Debug-Bridge end-to-end verifiziert

## Checkliste

| Bereich | Aktueller Status | Drift gefunden | Historisch behalten oder aktiv aktualisieren | Nachgezogen | Subagent-geprueft |
| --- | --- | --- | --- | --- | --- |
| Root / Meta | auf aktuellen Prototypstand gezogen | ja | aktiv aktualisieren | ja | ja |
| preparation/ | auf Custom-FQBN- und Prototypstand gezogen | ja | aktiv aktualisieren | ja | ja |
| hardware/ | aktueller Prototyp plus Historie getrennt | ja | teils historisch, teils aktiv | ja | ja |
| docs/ | Nachweise und Templates auf aktuellen Stand gezogen | ja | dated Logs historisch, Nachweise aktiv | ja | ja |
| firmware/ | aktiver Steuer- und Debugpfad dokumentiert | ja | teils historisch, teils aktiv | ja | ja |
| calibration/ | Greifer-Eingabe aktiv, Flex historisch gerahmt | ja | Flex-Sensor historisch, Greifer-Eingabe aktiv | ja | ja |
| tests/ | verifizierten Twin-/Bridge-Stand eingeordnet | ja | aktiv aktualisieren | ja | ja |
| security/ | Debugpfad vs. Steuerpfad klar getrennt | ja | aktiv aktualisieren | ja | ja |
| dashboard/ | aktuelles Twin-Mapping und Solver-Status dokumentiert | ja | aktiv aktualisieren | ja | ja |
| ros2/ | Quickstart auf Live-Debugpfad und Mapping aktualisiert | ja | aktiv aktualisieren | ja | ja |
| scripts/ | Snapshot auf Markdown plus Medien erweitert | ja | aktiv aktualisieren | ja | ja |
| future/ | auf echte Zukunftsthemen zurechtgestutzt | ja | in echte Zukunftsthemen rueckschneiden | ja | ja |
| official_downloads/ | Vendor-Rolle explizit vom Projektstand getrennt | ja | Vendor historisch, Projektabweichung aktiv | ja | ja |
| documentation/ | Snapshot aus aktualisierten Quellen neu erzeugt | ja | nur generiert, nicht direkt pflegen | ja | ja |

## Pflichtpunkte fuer die Abnahme

- Root- und Bereichsdoku verwenden konsistent die Formulierung `Potentiometer aus Robustheitsgruenden`.
- Historische Flex-Sensor- oder Breadboard-Staende sind klar als Altstand markiert.
- `HELP.md`, `AGENTS.md` und `GLOBAL_RULES.md` verweisen konsistent auf das Custom-Board `esp32:esp32:robotic_arm_s3n16r8`.
- Root-README enthaelt Demo-GIFs und verweist auf Dashboard- und ROS-Twin als verifizierten Digital-Twin-Pfad.
- `scripts/update_docs.sh` spiegelt Markdown plus README-relevante Medien in `documentation/snapshot/`.
