import math


REFERENCE_POSE = {
    's0': {'h': 255.0, 'r': -5.5, 'p': 65.8},
    's1': {'h': 260.3, 'r': -10.7, 'p': 86.0},
    's2': {'h': 280.8, 'r': 13.2, 'p': 88.9},
}

BASE_LIFT_MIN = 0.0
BASE_LIFT_MAX = 100.0
SHOULDER_HEADING_COEFF = 1.0
SHOULDER_INPUT_MIN = 0.0
SHOULDER_INPUT_MAX = 60.0
ELBOW_INPUT_MIN = 0.0
ELBOW_INPUT_MAX = 45.0
WRIST_RESPONSE_GAIN = 1.0
GRIPPER_INPUT_MIN = 0.0
GRIPPER_INPUT_MAX = 100.0
GRIPPER_FILTER_ALPHA = 0.22
GRIPPER_ANGLE_DEADBAND = 0.025

JOINT_LIMITS = {
    'base': (-math.pi / 2, math.pi / 2),
    'shoulder': (math.radians(-5.0), math.radians(80.0)),
    'elbow': (math.radians(-12.0), math.radians(120.0)),
    'wrist': (-math.pi / 2, math.pi / 2),
    'gripper': (0.0, 0.6),
}


def clamp(value, min_value, max_value):
    return max(min_value, min(max_value, value))


def clamp_joint(value, joint_name):
    lower, upper = JOINT_LIMITS[joint_name]
    return clamp(value, lower, upper)


def deg2rad(value_deg):
    return math.radians(value_deg)


def rad2deg(value_rad):
    return math.degrees(value_rad)


def normalize_heading_delta(current_deg, reference_deg):
    delta = current_deg - reference_deg
    while delta > 180.0:
        delta -= 360.0
    while delta < -180.0:
        delta += 360.0
    return delta


def map_linear(value, in_min, in_max, out_min, out_max):
    if in_max == in_min:
        return out_min
    return out_min + ((value - in_min) / (in_max - in_min)) * (out_max - out_min)


def stabilize_gripper_angle(target_angle, previous_angle):
    if previous_angle is None:
        return target_angle
    if abs(target_angle - previous_angle) < GRIPPER_ANGLE_DEADBAND:
        return previous_angle
    return previous_angle + ((target_angle - previous_angle) * GRIPPER_FILTER_ALPHA)


def quaternion_multiply(a, b):
    ax, ay, az, aw = a
    bx, by, bz, bw = b
    return (
        aw * bx + ax * bw + ay * bz - az * by,
        aw * by - ax * bz + ay * bw + az * bx,
        aw * bz + ax * by - ay * bx + az * bw,
        aw * bw - ax * bx - ay * by - az * bz,
    )


def quaternion_normalize(q):
    x, y, z, w = q
    norm = math.sqrt(x * x + y * y + z * z + w * w)
    if norm == 0.0:
        return (0.0, 0.0, 0.0, 1.0)
    return (x / norm, y / norm, z / norm, w / norm)


def quaternion_inverse(q):
    x, y, z, w = quaternion_normalize(q)
    return (-x, -y, -z, w)


def axis_angle_quaternion(x, y, z, angle_rad):
    half = angle_rad / 2.0
    s = math.sin(half)
    return (x * s, y * s, z * s, math.cos(half))


def sensor_quaternion(sensor):
    qx = axis_angle_quaternion(1.0, 0.0, 0.0, deg2rad(sensor['r']))
    qy = axis_angle_quaternion(0.0, 1.0, 0.0, deg2rad(sensor['p']))
    qz = axis_angle_quaternion(0.0, 0.0, 1.0, deg2rad(sensor['h']))
    return quaternion_normalize(quaternion_multiply(quaternion_multiply(qx, qy), qz))


def relative_wrist_twist_deg(hand_sensor, forearm_sensor):
    q_hand = sensor_quaternion(hand_sensor)
    q_forearm = sensor_quaternion(forearm_sensor)
    q_rel = quaternion_multiply(quaternion_inverse(q_forearm), q_hand)

    twist = quaternion_normalize((q_rel[0], 0.0, 0.0, q_rel[3]))
    twist_deg = rad2deg(2.0 * math.atan2(twist[0], twist[3]))
    return normalize_heading_delta(twist_deg, 0.0)


REFERENCE_REL = {
    'elbow_heading': normalize_heading_delta(REFERENCE_POSE['s1']['h'], REFERENCE_POSE['s2']['h']),
    'wrist_twist': relative_wrist_twist_deg(REFERENCE_POSE['s0'], REFERENCE_POSE['s1']),
}


def map_imu_payload(data, include_debug=False):
    sensors = data.get('s', [])
    if len(sensors) < 3:
        raise ValueError('IMU payload requires 3 sensors')

    hand = sensors[0]
    forearm = sensors[1]
    upper_arm = sensors[2]

    base_pitch_input = clamp(
        REFERENCE_POSE['s2']['p'] - upper_arm['p'],
        BASE_LIFT_MIN,
        BASE_LIFT_MAX,
    )
    base_yaw_deg = map_linear(
        base_pitch_input,
        BASE_LIFT_MIN,
        BASE_LIFT_MAX,
        rad2deg(JOINT_LIMITS['base'][1]),
        rad2deg(JOINT_LIMITS['base'][0]),
    )

    shoulder_heading_delta = normalize_heading_delta(upper_arm['h'], REFERENCE_POSE['s2']['h'])
    shoulder_input = -(shoulder_heading_delta * SHOULDER_HEADING_COEFF)
    shoulder_pitch_deg = map_linear(
        clamp(shoulder_input, SHOULDER_INPUT_MIN, SHOULDER_INPUT_MAX),
        SHOULDER_INPUT_MIN,
        SHOULDER_INPUT_MAX,
        0.0,
        rad2deg(JOINT_LIMITS['shoulder'][1]),
    )

    elbow_heading_rel = normalize_heading_delta(forearm['h'], upper_arm['h'])
    elbow_input = normalize_heading_delta(REFERENCE_REL['elbow_heading'], elbow_heading_rel)
    elbow_angle_deg = map_linear(
        clamp(elbow_input, ELBOW_INPUT_MIN, ELBOW_INPUT_MAX),
        ELBOW_INPUT_MIN,
        ELBOW_INPUT_MAX,
        0.0,
        rad2deg(JOINT_LIMITS['elbow'][1]),
    )

    wrist_twist_rel = relative_wrist_twist_deg(hand, forearm)
    wrist_angle_deg = normalize_heading_delta(
        wrist_twist_rel,
        REFERENCE_REL['wrist_twist'],
    ) * WRIST_RESPONSE_GAIN

    flex = clamp(float(data.get('f', GRIPPER_INPUT_MIN)), GRIPPER_INPUT_MIN, GRIPPER_INPUT_MAX)
    flex_norm = map_linear(flex, GRIPPER_INPUT_MIN, GRIPPER_INPUT_MAX, 0.0, 100.0)

    joint_positions = [
        clamp_joint(deg2rad(base_yaw_deg), 'base'),
        clamp_joint(deg2rad(shoulder_pitch_deg), 'shoulder'),
        clamp_joint(deg2rad(elbow_angle_deg), 'elbow'),
        clamp_joint(deg2rad(wrist_angle_deg), 'wrist'),
        clamp_joint((1.0 - (flex_norm / 100.0)) * 0.6, 'gripper'),
    ]

    if not include_debug:
        return joint_positions

    return joint_positions, {
        'sensors': {
            's0': hand,
            's1': forearm,
            's2': upper_arm,
        },
        'reference': {
            's2_pitch': REFERENCE_POSE['s2']['p'],
            's2_heading': REFERENCE_POSE['s2']['h'],
            'elbow_heading': REFERENCE_REL['elbow_heading'],
            'wrist_twist': REFERENCE_REL['wrist_twist'],
        },
        'base': {
            'pitch_input': base_pitch_input,
            'yaw_deg': base_yaw_deg,
        },
        'shoulder': {
            'heading_delta': shoulder_heading_delta,
            'input': shoulder_input,
            'pitch_deg': shoulder_pitch_deg,
        },
        'elbow': {
            'heading_rel': elbow_heading_rel,
            'input': elbow_input,
            'angle_deg': elbow_angle_deg,
        },
        'wrist': {
            'twist_rel_deg': wrist_twist_rel,
            'angle_deg': wrist_angle_deg,
        },
        'gripper': {
            'flex': flex,
            'flex_norm': flex_norm,
        },
        'joint_positions': {
            'base': joint_positions[0],
            'shoulder': joint_positions[1],
            'elbow': joint_positions[2],
            'wrist': joint_positions[3],
            'gripper': joint_positions[4],
        },
    }


def map_imu_payload_to_joint_positions(data):
    return map_imu_payload(data, include_debug=False)
