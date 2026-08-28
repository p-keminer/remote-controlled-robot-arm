#ifndef LIVE_I2C_MAPPING_H
#define LIVE_I2C_MAPPING_H

#include <Arduino.h>
#include <math.h>

// Fachliches Mapping fuer den spaeteren Live-I2C-Pfad.
// Die Konstanten spiegeln den aktuell verifizierten Dashboard-/ROS-Twin-Stand.

typedef struct {
    uint8_t basis;
    uint8_t schulter;
    uint8_t ellbogen;
    uint8_t handgelenk;
    uint8_t greifer;
    float handgelenk_raw_deg;
    float handgelenk_deg;
} LiveI2cZiele;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} LiveQuaternion;

static const SensorDaten REFERENCE_POSE[ANZAHL_SENSOREN] = {
    {255.0f, -5.5f, 65.8f},   // S0 = Hand/Wrist
    {260.3f, -10.7f, 86.0f},  // S1 = Unterarm
    {280.8f, 13.2f, 88.9f},   // S2 = Oberarm
};

#define LIVE_BASE_LIFT_MIN       0.0f
#define LIVE_BASE_LIFT_MAX     100.0f
#define LIVE_SHOULDER_INPUT_MIN -60.0f
#define LIVE_SHOULDER_INPUT_MAX 60.0f
#define LIVE_SHOULDER_REFERENCE_HEADING 240.0f
#define LIVE_ELBOW_INPUT_MIN     0.0f
#define LIVE_ELBOW_INPUT_MAX    45.0f
#define LIVE_WRIST_MIN_DEG     -90.0f
#define LIVE_WRIST_MAX_DEG      90.0f
#define LIVE_WRIST_CENTER_OFFSET_DEG 10.2f
#define LIVE_WRIST_POSITIVE_GAIN      1.0f
#define LIVE_WRIST_NEGATIVE_GAIN      1.0f
#define LIVE_WRIST_ABS_FALLBACK_MIN_DEG 35.0f
#define LIVE_WRIST_ABS_FALLBACK_REL_MAX_DEG 25.0f
#define LIVE_WRIST_WRAP_REJECT_DEG   90.0f
#define LIVE_WRIST_SAFE_MIN_DEG     -75.0f
#define LIVE_WRIST_SAFE_MAX_DEG      75.0f
#define LIVE_WRIST_FOLD_PITCH_ABS_DEG 145.0f
#define LIVE_WRIST_FOLD_LATCH_MIN_DEG 30.0f
#define LIVE_WRIST_FOLD_MARGIN_DEG     3.0f
#define LIVE_WRIST_FOLD_RELEASE_MARGIN_DEG 1.0f

static bool live_wrist_initialized = false;
static bool live_wrist_zero_pending = false;
static bool live_wrist_zero_done = false;
static bool live_wrist_fold_latched = false;
static float live_wrist_fold_sign = 0.0f;
static float live_wrist_fold_hold_deg = 0.0f;
static float live_wrist_center_offset_deg = LIVE_WRIST_CENTER_OFFSET_DEG;
static float live_wrist_last_deg = 0.0f;
static uint8_t live_shoulder_last_byte = 128;
static bool live_elbow_initialized = false;
static uint8_t live_elbow_last_byte = 128;

static float clamp_float(float wert, float min_wert, float max_wert) {
    if (wert < min_wert) return min_wert;
    if (wert > max_wert) return max_wert;
    return wert;
}

static float normalize_heading_delta(float current_deg, float reference_deg) {
    float delta = current_deg - reference_deg;
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;
    return delta;
}

static float map_linear(float wert, float in_min, float in_max, float out_min, float out_max) {
    if (in_max == in_min) return out_min;
    return out_min + ((wert - in_min) / (in_max - in_min)) * (out_max - out_min);
}

static uint8_t byte_aus_float(float wert, float in_min, float in_max) {
    float byte_wert = map_linear(clamp_float(wert, in_min, in_max), in_min, in_max, 0.0f, 255.0f);
    if (byte_wert < 0.0f) return 0;
    if (byte_wert > 255.0f) return 255;
    return (uint8_t)(byte_wert + 0.5f);
}

static LiveQuaternion quaternion_multiply(LiveQuaternion a, LiveQuaternion b) {
    LiveQuaternion q;
    q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return q;
}

static LiveQuaternion quaternion_normalize(LiveQuaternion q) {
    float norm = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (norm == 0.0f) return {0.0f, 0.0f, 0.0f, 1.0f};
    return {q.x / norm, q.y / norm, q.z / norm, q.w / norm};
}

static LiveQuaternion quaternion_inverse(LiveQuaternion q) {
    LiveQuaternion n = quaternion_normalize(q);
    return {-n.x, -n.y, -n.z, n.w};
}

static LiveQuaternion axis_angle_quaternion(float x, float y, float z, float angle_rad) {
    float half = angle_rad / 2.0f;
    float s = sinf(half);
    return {x * s, y * s, z * s, cosf(half)};
}

static LiveQuaternion sensor_quaternion(const SensorDaten* sensor) {
    LiveQuaternion qx = axis_angle_quaternion(1.0f, 0.0f, 0.0f, radians(sensor->roll));
    LiveQuaternion qy = axis_angle_quaternion(0.0f, 1.0f, 0.0f, radians(sensor->pitch));
    LiveQuaternion qz = axis_angle_quaternion(0.0f, 0.0f, 1.0f, radians(sensor->heading));
    return quaternion_normalize(quaternion_multiply(quaternion_multiply(qx, qy), qz));
}

static float relative_wrist_twist_deg(const SensorDaten* hand_sensor, const SensorDaten* forearm_sensor) {
    LiveQuaternion q_hand = sensor_quaternion(hand_sensor);
    LiveQuaternion q_forearm = sensor_quaternion(forearm_sensor);
    LiveQuaternion q_rel = quaternion_multiply(quaternion_inverse(q_forearm), q_hand);
    LiveQuaternion twist = quaternion_normalize({q_rel.x, 0.0f, 0.0f, q_rel.w});
    float twist_deg = degrees(2.0f * atan2f(twist.x, twist.w));
    return normalize_heading_delta(twist_deg, 0.0f);
}

static uint8_t basis_aus_imu(const ImuPaket* paket) {
    float base_pitch_input = clamp_float(
        REFERENCE_POSE[2].pitch - paket->sensoren[2].pitch,
        LIVE_BASE_LIFT_MIN,
        LIVE_BASE_LIFT_MAX
    );
    float base_yaw_deg = map_linear(base_pitch_input, LIVE_BASE_LIFT_MIN, LIVE_BASE_LIFT_MAX, -90.0f, 90.0f);
    return byte_aus_float(base_yaw_deg, -90.0f, 90.0f);
}

static uint8_t schulter_aus_imu(const ImuPaket* paket) {
    bool s2_nullframe =
        paket->sensoren[2].heading == 0.0f &&
        paket->sensoren[2].roll == 0.0f &&
        paket->sensoren[2].pitch == 0.0f;

    if (s2_nullframe) {
        return live_shoulder_last_byte;
    }

    float delta = normalize_heading_delta(paket->sensoren[2].heading, LIVE_SHOULDER_REFERENCE_HEADING);
    float input = delta;
    float schulter_deg = map_linear(
        clamp_float(input, LIVE_SHOULDER_INPUT_MIN, LIVE_SHOULDER_INPUT_MAX),
        LIVE_SHOULDER_INPUT_MIN,
        LIVE_SHOULDER_INPUT_MAX,
        0.0f,
        80.0f
    );
    uint8_t schulter_byte = byte_aus_float(schulter_deg, 0.0f, 80.0f);
    if (!s2_nullframe) {
        live_shoulder_last_byte = schulter_byte;
    }
    return schulter_byte;
}

static uint8_t ellbogen_aus_imu(const ImuPaket* paket) {
    bool s1_nullframe =
        paket->sensoren[1].heading == 0.0f &&
        paket->sensoren[1].roll == 0.0f &&
        paket->sensoren[1].pitch == 0.0f;
    bool s2_nullframe =
        paket->sensoren[2].heading == 0.0f &&
        paket->sensoren[2].roll == 0.0f &&
        paket->sensoren[2].pitch == 0.0f;

    if ((s1_nullframe || s2_nullframe) && live_elbow_initialized) {
        return live_elbow_last_byte;
    }

    float referenz_rel = normalize_heading_delta(REFERENCE_POSE[1].heading, REFERENCE_POSE[2].heading);
    float aktuell_rel = normalize_heading_delta(paket->sensoren[1].heading, paket->sensoren[2].heading);
    float input = normalize_heading_delta(referenz_rel, aktuell_rel);
    float ellbogen_deg = map_linear(
        clamp_float(input, LIVE_ELBOW_INPUT_MIN, LIVE_ELBOW_INPUT_MAX),
        LIVE_ELBOW_INPUT_MIN,
        LIVE_ELBOW_INPUT_MAX,
        0.0f,
        120.0f
    );
    uint8_t ellbogen_byte = byte_aus_float(ellbogen_deg, 0.0f, 120.0f);
    if (!s1_nullframe && !s2_nullframe) {
        live_elbow_last_byte = ellbogen_byte;
        live_elbow_initialized = true;
    }
    return ellbogen_byte;
}

static float handgelenk_raw_deg_aus_imu(const ImuPaket* paket) {
    // Fuer die echte Hardware ist die Quaternion-/Euler-Twist-Ableitung im
    // rechten Grenzbereich instabil, weil die BNO055-Eulerwerte dort springen.
    // Die relative Roll-Differenz S0-S1 bleibt in unseren Livewerten stabiler.
    float relative_roll = -normalize_heading_delta(paket->sensoren[0].roll, paket->sensoren[1].roll);

    // Daumen-nach-unten-Zone: In dieser Pose rollt S1 mit und die relative
    // Roll-Differenz hebt die echte Handdrehung fast weg. Dann ist S0.roll
    // gegen die Hand-Referenz der bessere Proxy.
    float absolute_hand_roll = normalize_heading_delta(paket->sensoren[0].roll, REFERENCE_POSE[0].roll);
    if (absolute_hand_roll > LIVE_WRIST_ABS_FALLBACK_MIN_DEG
        && fabsf(relative_roll) < LIVE_WRIST_ABS_FALLBACK_REL_MAX_DEG) {
        return -absolute_hand_roll;
    }

    return relative_roll;
}

static bool handgelenk_fold_zone_aktiv(const SensorDaten* hand_sensor) {
    if (hand_sensor == NULL) return false;
    return fabsf(hand_sensor->pitch) >= LIVE_WRIST_FOLD_PITCH_ABS_DEG;
}

static float handgelenk_deg_stabilisieren(float raw_deg, const SensorDaten* hand_sensor) {
    if (live_wrist_zero_pending) {
        live_wrist_center_offset_deg = -raw_deg;
        live_wrist_initialized = false;
        live_wrist_zero_pending = false;
        live_wrist_zero_done = true;
        live_wrist_fold_latched = false;
        live_wrist_fold_sign = 0.0f;
        live_wrist_fold_hold_deg = 0.0f;
    }

    float adjusted = normalize_heading_delta(raw_deg + live_wrist_center_offset_deg, 0.0f);
    adjusted *= (adjusted >= 0.0f) ? LIVE_WRIST_POSITIVE_GAIN : LIVE_WRIST_NEGATIVE_GAIN;
    adjusted = clamp_float(adjusted, LIVE_WRIST_SAFE_MIN_DEG, LIVE_WRIST_SAFE_MAX_DEG);
    bool fold_zone = handgelenk_fold_zone_aktiv(hand_sensor);

    if (!live_wrist_initialized) {
        live_wrist_last_deg = adjusted;
        live_wrist_initialized = true;
        return adjusted;
    }

    float delta = normalize_heading_delta(adjusted, live_wrist_last_deg);
    if (fabsf(delta) > LIVE_WRIST_WRAP_REJECT_DEG) {
        adjusted = live_wrist_last_deg;
    }

    if (live_wrist_fold_latched) {
        bool zurueck_ueber_grenze = false;
        if (live_wrist_fold_sign > 0.0f) {
            zurueck_ueber_grenze = adjusted < (live_wrist_fold_hold_deg - LIVE_WRIST_FOLD_RELEASE_MARGIN_DEG);
        } else if (live_wrist_fold_sign < 0.0f) {
            zurueck_ueber_grenze = adjusted > (live_wrist_fold_hold_deg + LIVE_WRIST_FOLD_RELEASE_MARGIN_DEG);
        }

        if (!fold_zone && zurueck_ueber_grenze) {
            live_wrist_fold_latched = false;
            live_wrist_fold_sign = 0.0f;
            live_wrist_fold_hold_deg = 0.0f;
        } else {
            live_wrist_last_deg = live_wrist_fold_hold_deg;
            return live_wrist_last_deg;
        }
    }

    if (fold_zone && fabsf(live_wrist_last_deg) >= LIVE_WRIST_FOLD_LATCH_MIN_DEG) {
        live_wrist_fold_sign = (live_wrist_last_deg < 0.0f) ? -1.0f : 1.0f;
        live_wrist_fold_hold_deg = clamp_float(
            live_wrist_last_deg + (live_wrist_fold_sign * LIVE_WRIST_FOLD_MARGIN_DEG),
            LIVE_WRIST_SAFE_MIN_DEG,
            LIVE_WRIST_SAFE_MAX_DEG
        );
        live_wrist_fold_latched = true;
        live_wrist_last_deg = live_wrist_fold_hold_deg;
        return live_wrist_last_deg;
    }

    delta = normalize_heading_delta(adjusted, live_wrist_last_deg);
    live_wrist_last_deg = clamp_float(live_wrist_last_deg + delta, LIVE_WRIST_SAFE_MIN_DEG, LIVE_WRIST_SAFE_MAX_DEG);
    return live_wrist_last_deg;
}

static void live_wrist_zero_anfordern() {
    live_wrist_zero_pending = true;
}

static uint8_t handgelenk_byte_aus_deg(float handgelenk_deg) {
    return byte_aus_float(handgelenk_deg, LIVE_WRIST_MIN_DEG, LIVE_WRIST_MAX_DEG);
}

static uint8_t greifer_aus_prozent(float greifer_prozent) {
    return byte_aus_float(greifer_prozent, 0.0f, 100.0f);
}

static bool live_i2c_mapping_berechnen(const ImuPaket* paket, LiveI2cZiele* ziele) {
    if (paket == NULL || ziele == NULL) return false;
    ziele->basis = basis_aus_imu(paket);
    ziele->schulter = schulter_aus_imu(paket);
    ziele->ellbogen = ellbogen_aus_imu(paket);
    ziele->handgelenk_raw_deg = handgelenk_raw_deg_aus_imu(paket);
    ziele->handgelenk_deg = handgelenk_deg_stabilisieren(ziele->handgelenk_raw_deg, &paket->sensoren[0]);
    ziele->handgelenk = handgelenk_byte_aus_deg(ziele->handgelenk_deg);
    ziele->greifer = greifer_aus_prozent(paket->flex_prozent);
    return true;
}

#endif
