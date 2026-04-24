#!/usr/bin/env python3

import argparse
import json
import math
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


def load_jsonl(path):
    rows = []
    with open(path, 'r', encoding='utf-8') as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue
            rows.append(json.loads(line))
    return rows


def normalize_times(rows):
    if not rows:
        return []
    start_ns = int(rows[0].get('stamp_ns', 0))
    return [
        (int(row.get('stamp_ns', start_ns)) - start_ns) / 1_000_000_000.0
        for row in rows
    ]


def row_value(row, path, default=None):
    current = row
    for key in path:
        if isinstance(key, int):
            if not isinstance(current, list) or key < 0 or key >= len(current):
                return default
            current = current[key]
        else:
            if not isinstance(current, dict) or key not in current:
                return default
            current = current[key]
    return current


def extract_series(rows, path, default=float('nan')):
    values = []
    for row in rows:
        value = row_value(row, path, default)
        if value is None:
            value = default
        values.append(value)
    return values


def rad_to_deg_list(values):
    return [
        math.degrees(value) if isinstance(value, (float, int)) else float('nan')
        for value in values
    ]


def latest_session_dir(base_dir):
    candidates = [path for path in Path(base_dir).iterdir() if path.is_dir()]
    if not candidates:
        raise FileNotFoundError(f'No debug sessions found in {base_dir}')
    return sorted(candidates)[-1]


def resolve_session_dir(session_dir):
    path = Path(session_dir)
    if path.is_dir():
        return path
    if path.name == 'raw_imu.jsonl':
        return path.parent
    raise FileNotFoundError(session_dir)


def create_plot(session_dir, output_file=None):
    session_dir = resolve_session_dir(session_dir)
    raw_rows = load_jsonl(session_dir / 'raw_imu.jsonl')
    mapping_rows = load_jsonl(session_dir / 'mapping.jsonl')
    joint_rows = load_jsonl(session_dir / 'joint_states.jsonl')

    if not raw_rows or not mapping_rows:
        raise ValueError(f'Session {session_dir} contains no debug data')

    raw_t = normalize_times(raw_rows)
    mapping_t = normalize_times(mapping_rows)
    joint_t = normalize_times(joint_rows) if joint_rows else []

    fig, axes = plt.subplots(4, 1, figsize=(15, 16), sharex=False)

    axes[0].plot(raw_t, extract_series(raw_rows, ['payload', 's', 2, 'p']), label='S2 pitch')
    axes[0].plot(raw_t, extract_series(raw_rows, ['payload', 's', 2, 'h']), label='S2 heading')
    axes[0].plot(raw_t, extract_series(raw_rows, ['payload', 's', 1, 'h']), label='S1 heading')
    axes[0].plot(raw_t, extract_series(raw_rows, ['payload', 'f']), label='Flex')
    axes[0].set_title('Rohdaten')
    axes[0].set_ylabel('Grad / Flex')
    axes[0].grid(True, alpha=0.3)
    axes[0].legend(loc='upper right')

    axes[1].plot(mapping_t, extract_series(mapping_rows, ['payload', 'base', 'yaw_deg']), label='Base yaw')
    axes[1].plot(mapping_t, extract_series(mapping_rows, ['payload', 'shoulder', 'pitch_deg']), label='Shoulder')
    axes[1].plot(mapping_t, extract_series(mapping_rows, ['payload', 'elbow', 'angle_deg']), label='Elbow')
    axes[1].plot(mapping_t, extract_series(mapping_rows, ['payload', 'wrist', 'angle_deg']), label='Wrist')
    axes[1].set_title('Gemappte Gelenkwinkel')
    axes[1].set_ylabel('Grad')
    axes[1].grid(True, alpha=0.3)
    axes[1].legend(loc='upper right')

    axes[2].plot(mapping_t, extract_series(mapping_rows, ['payload', 'base', 'pitch_input']), label='Base pitch input')
    axes[2].plot(mapping_t, extract_series(mapping_rows, ['payload', 'shoulder', 'input']), label='Shoulder input')
    axes[2].plot(mapping_t, extract_series(mapping_rows, ['payload', 'elbow', 'input']), label='Elbow input')
    axes[2].plot(mapping_t, extract_series(mapping_rows, ['payload', 'wrist', 'twist_rel_deg']), label='Wrist twist rel')
    axes[2].set_title('Zwischenwerte')
    axes[2].set_ylabel('Grad')
    axes[2].grid(True, alpha=0.3)
    axes[2].legend(loc='upper right')

    if joint_rows:
        joint_base = []
        joint_shoulder = []
        joint_elbow = []
        joint_wrist = []
        joint_gripper = []
        for row in joint_rows:
            names = row.get('name', [])
            positions = row.get('position', [])
            mapping = dict(zip(names, positions))
            base_value = mapping.get('base_joint_l', mapping.get('base_joint', mapping.get('base_joint_r', float('nan'))))
            shoulder_value = mapping.get('shoulder_joint_l', mapping.get('shoulder_joint', mapping.get('shoulder_joint_r', float('nan'))))
            elbow_value = mapping.get('elbow_joint_l', mapping.get('elbow_joint', mapping.get('elbow_joint_r', float('nan'))))
            wrist_value = mapping.get('wrist_joint_l', mapping.get('wrist_joint', mapping.get('wrist_joint_r', float('nan'))))
            gripper_value = mapping.get('gripper_joint_l', mapping.get('gripper_left_joint', mapping.get('gripper_joint_r', float('nan'))))
            joint_base.append(base_value)
            joint_shoulder.append(shoulder_value)
            joint_elbow.append(elbow_value)
            joint_wrist.append(wrist_value)
            joint_gripper.append(gripper_value)

        axes[3].plot(joint_t, rad_to_deg_list(joint_base), label='Base joint')
        axes[3].plot(joint_t, rad_to_deg_list(joint_shoulder), label='Shoulder joint')
        axes[3].plot(joint_t, rad_to_deg_list(joint_elbow), label='Elbow joint')
        axes[3].plot(joint_t, rad_to_deg_list(joint_wrist), label='Wrist joint')
        axes[3].plot(joint_t, joint_gripper, label='Gripper')
    axes[3].set_title('Publizierte JointStates')
    axes[3].set_xlabel('Zeit [s]')
    axes[3].set_ylabel('Grad / m')
    axes[3].grid(True, alpha=0.3)
    if joint_rows:
        axes[3].legend(loc='upper right')

    fig.suptitle(f'Robotarm Debug Session: {session_dir.name}', fontsize=16)
    fig.tight_layout()

    if output_file is None:
        output_file = session_dir / 'curves.png'
    else:
        output_file = Path(output_file)

    fig.savefig(output_file, dpi=160)
    plt.close(fig)
    return output_file


def main():
    parser = argparse.ArgumentParser(description='Erzeuge Kurvendiagramm aus einer Robotarm-Debug-Session.')
    parser.add_argument('session_dir', nargs='?', help='Pfad zur Session oder zu raw_imu.jsonl')
    parser.add_argument('--latest-under', dest='latest_under', help='Nimmt automatisch die neueste Session unter diesem Ordner')
    parser.add_argument('--output', dest='output', help='Optionaler Ausgabepfad fuer PNG')
    args = parser.parse_args()

    if args.session_dir:
        session_dir = resolve_session_dir(args.session_dir)
    elif args.latest_under:
        session_dir = latest_session_dir(args.latest_under)
    else:
        raise SystemExit('Bitte session_dir angeben oder --latest-under verwenden.')

    output_file = create_plot(session_dir, args.output)
    print(output_file)


if __name__ == '__main__':
    main()
