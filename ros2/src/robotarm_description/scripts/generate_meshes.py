#!/usr/bin/env python3
"""
Generiert STL-Meshes fuer den Adeept 5-DOF Roboterarm URDF.
Geometrie exakt aus Three.js robotarm_3d.js buildArm().
1 Three.js-Einheit = 10mm. Ausgabe in Metern (SI fuer ROS).

Koordinatensystem: Z ist oben (ROS-Konvention).
Three.js nutzt Y-oben → hier umgewandelt: Y_three → Z_ros, Z_three → -Y_ros
"""

import trimesh
import numpy as np
import os

# === Ausgabepfad ===
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MESH_DIR = os.path.join(SCRIPT_DIR, '..', 'meshes')
os.makedirs(MESH_DIR, exist_ok=True)

# === Konstanten (Three.js, 1 unit = 10mm → *0.01 fuer Meter) ===
S = 0.01  # Scale: 1 unit → 0.01m

BASE_W   = 10 * S
BASE_D   = 10 * S
BASE_H   = 1.5 * S
TURNTABLE_H = 0.8 * S
SHOULDER_H  = 6 * S
UPPER_ARM_L = 10.5 * S
FOREARM_L   = 9.5 * S
WRIST_L     = 3.0 * S
GRIPPER_L   = 5.5 * S
SERVO_W     = 2.3 * S
SERVO_H     = 1.2 * S
SERVO_D     = 4.0 * S
BRACKET_T   = 0.3 * S
BRACKET_W   = 3.5 * S
STANDOFF_R  = 0.15 * S


def make_box(w, h, d):
    """Box mit Zentrum bei (0,0,0). w=X, h=Z(oben), d=Y."""
    return trimesh.creation.box(extents=[w, d, h])


def make_cylinder(radius, height, segments=24):
    """Zylinder entlang Z-Achse, Zentrum bei (0,0,0)."""
    return trimesh.creation.cylinder(radius=radius, height=height, sections=segments)


def translate(mesh, x, y, z):
    """Verschiebe Mesh."""
    m = mesh.copy()
    m.apply_translation([x, y, z])
    return m


def rotate_x(mesh, angle):
    """Rotation um X-Achse."""
    m = mesh.copy()
    rot = trimesh.transformations.rotation_matrix(angle, [1, 0, 0])
    m.apply_transform(rot)
    return m


def rotate_z(mesh, angle):
    """Rotation um Z-Achse."""
    m = mesh.copy()
    rot = trimesh.transformations.rotation_matrix(angle, [0, 0, 1])
    m.apply_transform(rot)
    return m


def combine(meshes):
    """Kombiniere Liste von Meshes zu einem."""
    return trimesh.util.concatenate(meshes)


def make_servo():
    """Servo-Block + Horn + Axle."""
    body = make_box(SERVO_W, SERVO_H, SERVO_D)
    horn = make_cylinder(0.005, 0.0015, 12)
    horn = translate(horn, 0, 0, SERVO_H / 2 + 0.0008)
    axle = make_cylinder(0.002, SERVO_H + 0.004, 8)
    return combine([body, horn, axle])


def make_bracket_plate(w, h, thickness):
    """Bracket-Platte: thickness=X, h=Z, w=Y."""
    return make_box(thickness, h, w)


def make_dual_bracket(w, h, separation, with_standoffs=False):
    """Zwei parallele Platten mit optionalen Standoffs."""
    parts = []
    plate1 = make_bracket_plate(w, h, BRACKET_T)
    plate1 = translate(plate1, -separation / 2, 0, 0)
    parts.append(plate1)

    plate2 = make_bracket_plate(w, h, BRACKET_T)
    plate2 = translate(plate2, separation / 2, 0, 0)
    parts.append(plate2)

    if with_standoffs:
        for i in range(2):
            standoff = make_cylinder(STANDOFF_R, separation - BRACKET_T, 6)
            standoff = rotate_x(standoff, np.pi / 2)
            # Rotate to lie along X axis
            rot = trimesh.transformations.rotation_matrix(np.pi / 2, [0, 1, 0])
            standoff.apply_transform(rot)
            y_off = (1 if i == 0 else -1) * w * 0.35
            standoff = translate(standoff, 0, y_off, -h * 0.35)
            parts.append(standoff)

    return combine(parts)


# ============================================================
# MESH 1: BASE_LINK — Grundplatte + Logo-Platte
# Origin: Boden bei Z=0, Mitte bei XY=0
# ============================================================
def gen_base_link():
    # Grundplatte
    platform = make_box(BASE_W, BASE_H, BASE_D)
    platform = translate(platform, 0, 0, BASE_H / 2)

    # Logo-Platte (Zylinder oben drauf)
    logo = make_cylinder(0.037, 0.003, 32)
    logo = translate(logo, 0, 0, BASE_H + 0.0015)

    mesh = combine([platform, logo])
    mesh.export(os.path.join(MESH_DIR, 'base_link.stl'))
    print('  base_link.stl')


# ============================================================
# MESH 2: TURNTABLE — Drehscheibe
# Origin: Boden bei Z=0
# ============================================================
def gen_turntable():
    disc = make_cylinder(0.031, TURNTABLE_H, 32)
    disc = translate(disc, 0, 0, TURNTABLE_H / 2)

    mesh = disc
    mesh.export(os.path.join(MESH_DIR, 'turntable.stl'))
    print('  turntable.stl')


# ============================================================
# MESH 3: SHOULDER_BRACKET — Zwei Seitenplatten + Rueckplatte
#          + Servo + Horn + Axle
# Origin: Boden bei Z=0
# ============================================================
def gen_shoulder_bracket():
    parts = []

    # Linke Seitenplatte
    pl = make_bracket_plate(BRACKET_W, SHOULDER_H, BRACKET_T)
    pl = translate(pl, -0.020, 0, SHOULDER_H / 2)
    parts.append(pl)

    # Rechte Seitenplatte
    pr = make_bracket_plate(BRACKET_W, SHOULDER_H, BRACKET_T)
    pr = translate(pr, 0.020, 0, SHOULDER_H / 2)
    parts.append(pr)

    # Rueckplatte
    back = make_box(0.043, SHOULDER_H * 0.6, BRACKET_T)
    back = translate(back, 0, -BRACKET_W / 2, SHOULDER_H * 0.3)
    parts.append(back)

    # Schulter-Servo (rotiert um Z 90°, bei Z=SHOULDER_H)
    servo = make_servo()
    servo = rotate_z(servo, np.pi / 2)
    servo = translate(servo, 0, 0, SHOULDER_H)
    parts.append(servo)

    # Schulter-Axle (entlang X, bei Z=SHOULDER_H)
    axle = make_cylinder(0.0025, 0.045, 12)
    rot = trimesh.transformations.rotation_matrix(np.pi / 2, [0, 1, 0])
    axle.apply_transform(rot)
    axle = translate(axle, 0, 0, SHOULDER_H)
    parts.append(axle)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'shoulder_bracket.stl'))
    print('  shoulder_bracket.stl')


# ============================================================
# MESH 4: UPPER_ARM — Dual-Bracket + Standoffs + Ellbogen-Servo
# Origin: Gelenk bei Z=0, Arm geht nach Z+
# ============================================================
def gen_upper_arm():
    parts = []

    # Dual-Bracket
    bracket = make_dual_bracket(BRACKET_W, UPPER_ARM_L, 0.035, with_standoffs=True)
    bracket = translate(bracket, 0, 0, UPPER_ARM_L / 2)
    parts.append(bracket)

    # Ellbogen-Servo
    servo = make_servo()
    servo = rotate_z(servo, np.pi / 2)
    servo = translate(servo, 0, 0, UPPER_ARM_L)
    parts.append(servo)

    # Ellbogen-Axle
    axle = make_cylinder(0.0025, 0.040, 12)
    rot = trimesh.transformations.rotation_matrix(np.pi / 2, [0, 1, 0])
    axle.apply_transform(rot)
    axle = translate(axle, 0, 0, UPPER_ARM_L)
    parts.append(axle)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'upper_arm.stl'))
    print('  upper_arm.stl')


# ============================================================
# MESH 5: FOREARM — Schmalerer Dual-Bracket + Wrist-Servo
# Origin: Gelenk bei Z=0, Arm geht nach Z+
# ============================================================
def gen_forearm():
    parts = []

    # Dual-Bracket (schmaler: BRACKET_W - 0.5 units = 30mm)
    bracket = make_dual_bracket(0.030, FOREARM_L, 0.030, with_standoffs=True)
    bracket = translate(bracket, 0, 0, FOREARM_L / 2)
    parts.append(bracket)

    # Wrist-Servo
    servo = make_servo()
    servo = rotate_z(servo, np.pi / 2)
    servo = translate(servo, 0, 0, FOREARM_L)
    parts.append(servo)

    # Wrist-Axle
    axle = make_cylinder(0.002, 0.035, 12)
    rot = trimesh.transformations.rotation_matrix(np.pi / 2, [0, 1, 0])
    axle.apply_transform(rot)
    axle = translate(axle, 0, 0, FOREARM_L)
    parts.append(axle)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'forearm.stl'))
    print('  forearm.stl')


# ============================================================
# MESH 6: WRIST — Adapter + Greifer-Gehaeuse + Greifer-Servo
# Origin: Gelenk bei Z=0
# ============================================================
def gen_wrist():
    parts = []

    # Wrist-Adapter
    adapter = make_box(0.025, WRIST_L, 0.025)
    adapter = translate(adapter, 0, 0, WRIST_L / 2)
    parts.append(adapter)

    # Greifer-Gehaeuse
    housing = make_box(0.030, 0.015, 0.025)
    housing = translate(housing, 0, 0, WRIST_L + 0.0075)
    parts.append(housing)

    # Greifer-Servo (skaliert 0.7)
    servo_body = make_box(0.028, 0.0084, 0.0161)
    servo_body = rotate_z(servo_body, np.pi / 2)
    servo_body = translate(servo_body, 0, 0, WRIST_L + 0.005)
    parts.append(servo_body)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'wrist.stl'))
    print('  wrist.stl')


# ============================================================
# MESH 7: GRIPPER_LEFT — Finger + abgewinkelte Spitze
# Origin: Gelenk bei Z=0
# ============================================================
def gen_gripper_left():
    parts = []

    # Finger
    finger = make_box(0.004, GRIPPER_L, 0.010)
    finger = translate(finger, 0, 0, GRIPPER_L / 2)
    parts.append(finger)

    # Fingerspitze (rotiert -0.3 rad um Z, versetzt +X)
    tip = make_box(0.004, 0.012, 0.010)
    tip = rotate_z(tip, -0.3)
    tip = translate(tip, 0.003, 0, GRIPPER_L)
    parts.append(tip)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'gripper_left.stl'))
    print('  gripper_left.stl')


# ============================================================
# MESH 8: GRIPPER_RIGHT — Finger + Spitze (gespiegelt)
# Origin: Gelenk bei Z=0
# ============================================================
def gen_gripper_right():
    parts = []

    # Finger
    finger = make_box(0.004, GRIPPER_L, 0.010)
    finger = translate(finger, 0, 0, GRIPPER_L / 2)
    parts.append(finger)

    # Fingerspitze (rotiert +0.3 rad um Z, versetzt -X)
    tip = make_box(0.004, 0.012, 0.010)
    tip = rotate_z(tip, 0.3)
    tip = translate(tip, -0.003, 0, GRIPPER_L)
    parts.append(tip)

    mesh = combine(parts)
    mesh.export(os.path.join(MESH_DIR, 'gripper_right.stl'))
    print('  gripper_right.stl')


# ============================================================
if __name__ == '__main__':
    print('Generiere STL-Meshes fuer Adeept 5-DOF Arm...')
    gen_base_link()
    gen_turntable()
    gen_shoulder_bracket()
    gen_upper_arm()
    gen_forearm()
    gen_wrist()
    gen_gripper_left()
    gen_gripper_right()
    print(f'Fertig! Meshes in: {os.path.abspath(MESH_DIR)}')
