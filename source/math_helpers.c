#include <math.h>
#include "math_helpers.h"
#include "color_channels.h"

#include <float.h>

Vec2D normalize(Vec2D v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    return (Vec2D){ v.x / len, v.y / len };
}

float dot_vec(Vec2D a, Vec2D b) {
    return a.x * b.x + a.y * b.y;
}

Vec2D perpendicular(Vec2D v) {
    return (Vec2D){ -v.y, v.x };
}


float clampf(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

float repeat(float a, float length) {
	return clampf(a - floorf(a / length) * length, 0.0f, length);
}

float positive_fmodf(float n, float divisor) {
    float value = fmodf(n, divisor);
    return value + (value < 0 ? divisor : 0);
}

Color color_lerp(Color color1, Color color2, float fraction) {
    unsigned char r1 = color1.r;
    unsigned char r2 = color2.r;
    unsigned char g1 = color1.g;
    unsigned char g2 = color2.g;
    unsigned char b1 = color1.b;
    unsigned char b2 = color2.b;

    Color returned;
    returned.r = (r2 - r1) * fraction + r1;
    returned.g = (g2 - g1) * fraction + g1;
    returned.b = (b2 - b1) * fraction + b1;

    return returned;
}

// Generic easing out function, not from GD
float ease_out(float current, float target, float smoothing) {
    return current + (target - current) * smoothing;
}

float map_range(float val, float min1, float max1, float min2, float max2) {
    return min2 + ((max2 - min2) / (max1 - min1)) * (val - min1);
}

float slerp(float a, float b, float ratio) {
	float delta = repeat((b - a), 360.f);
	if (delta > 180.f)
		delta -= 360.f;
	return a + delta * clampf(ratio, 0.f, 1.f);
}

float slerp_fancy(float fromAngle, float toAngle, float t) {
    float fromHalf = fromAngle * 0.5f;
    float toHalf   = toAngle   * 0.5f;

    float fx = cosf(fromHalf);
    float fy = sinf(fromHalf);
    float tx = cosf(toHalf);
    float ty = sinf(toHalf);

    float dot = fx * tx + fy * ty;

    if (dot < 0.0f) {
        dot = -dot;
        tx = -tx;
        ty = -ty;
    }

    float w0 = 1.0f - t;
    float w1 = t;

    if (dot < 0.9999f) {
        float between = acosf(dot);
        float sinBetween = sinf(between);

        w0 = sinf(w0 * between) / sinBetween;
        w1 = sinf(w1 * between) / sinBetween;
    }

    float ix = w0 * fx + w1 * tx;
    float iy = w0 * fy + w1 * ty;

    return atan2f(iy, ix) * 2.0f;
}

float lerp(float from, float to, float alpha) {
    return from * (1.0f - alpha) + to * alpha;
}

float iLerp(float a, float b, float ratio, float dt) {
	const float rDelta = dt * STEPS_HZ;
	const float s	  = 1.f - ratio;

	float iRatio = 1.f - powf(s, rDelta);

	return lerp(a, b, iRatio);
}

float iSlerp(float a, float b, float ratio, float dt) {
	const float rDelta = dt * STEPS_HZ;
	const float s	  = 1.f - ratio;

	float iRatio = 1.f - powf(s, rDelta);

	return slerp(a, b, iRatio);
}

bool is_effectively_integer(float x) {
    return fabsf(x - roundf(x)) < FLT_EPSILON;
}

float ip1_ceilf(float x) {
    float c = ceilf(x);
    if (is_effectively_integer(x)) {
        return c + 1.0f;
    }
    return c;
}


float adjust_angle(float angle, int flipX, int flipY) {
    // Normalize to [0, 360)
    angle = positive_fmodf(angle, 360);

    if (flipX && flipY) {
        angle = fmodf(angle + 180.0f, 360.0f);
    } else if (flipX) {
        angle = 180.0f - angle;
    } else if (flipY) {
        angle = -angle;
    }

    // Normalize again
    angle = positive_fmodf(angle, 360);

    return angle;
}

float adjust_angle_y(float angle, int flipY) {
    angle = positive_fmodf(angle, 360);

    if (flipY) {
        angle = angle + 180;
    }

    angle = positive_fmodf(angle, 360);

    return angle;
}


float square_distance(float xa, float ya, float xb, float yb) {
	return ((xb - xa) * (xb - xa)) + ((yb - ya) * (yb - ya));
}