from sensor_msgs.msg import JointState


SINGLE_ARM_NAMES = [
    'base_joint',
    'shoulder_joint',
    'elbow_joint',
    'wrist_joint',
    'gripper_left_joint',
]

WALL_FRONT_NAMES = [
    'base_joint_l',
    'shoulder_joint_l',
    'elbow_joint_l',
    'wrist_joint_l',
    'gripper_joint_l',
]

WALL_BACK_NAMES = [
    'base_joint_r',
    'shoulder_joint_r',
    'elbow_joint_r',
    'wrist_joint_r',
    'gripper_joint_r',
]


def joint_name_sets(wall_mode):
    if wall_mode:
        return WALL_FRONT_NAMES[:], WALL_BACK_NAMES[:]
    return SINGLE_ARM_NAMES[:], SINGLE_ARM_NAMES[:]


def build_joint_state(stamp, joint_positions, wall_mode=False, target_arm='front'):
    joint_state = JointState()
    joint_state.header.stamp = stamp

    joint_names_front, joint_names_back = joint_name_sets(wall_mode)

    if wall_mode:
        inactive = [0.0] * len(joint_names_front)
        joint_state.name = joint_names_front + joint_names_back
        if target_arm == 'back':
            joint_state.position = inactive + joint_positions
        elif target_arm == 'both':
            joint_state.position = joint_positions + joint_positions
        else:
            joint_state.position = joint_positions + inactive
    else:
        joint_state.name = joint_names_front
        joint_state.position = joint_positions

    return joint_state
