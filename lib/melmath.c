//
//  melmath.c
//  shmup
//
//  Created by Raphaël Calabro on 11/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "melmath.h"

int MELFloatSign(float value) {
    return (value > 0) - (value < 0);
}

float MELFloatMin(float lhs, float rhs) {
    return lhs < rhs ? lhs : rhs;
}

uint8_t MELUInt8Min(uint8_t lhs, uint8_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

uint8_t MELUInt8Max(uint8_t lhs, uint8_t rhs) {
    return lhs > rhs ? lhs : rhs;
}

float MELFloatMax(float lhs, float rhs) {
    return lhs > rhs ? lhs : rhs;
}
float MELFloatMax3(float a, float b, float c) {
    return (a > b && a > c)
        ? a
        : (b > c)
            ? b
            : c;
}

float MELFloatBound(float min, float value, float max) {
    return value < min ? min : (value < max ? value : max);
}

int32_t MELIntBound(int32_t min, int32_t value, int32_t max) {
    return value < min ? min : (value < max ? value : max);
}

int MELIntMax(int lhs, int rhs) {
    return lhs > rhs ? lhs : rhs;
}

int32_t MELInt32Max(int32_t lhs, int32_t rhs) {
    return lhs > rhs ? lhs : rhs;
}

uint32_t MELUInt32Max(uint32_t lhs, uint32_t rhs) {
    return lhs > rhs ? lhs : rhs;
}

int64_t MELInt64Max(int64_t lhs, int64_t rhs) {
    return lhs > rhs ? lhs : rhs;
}

int MELIntMin(int lhs, int rhs) {
    return lhs < rhs ? lhs : rhs;
}

uint32_t MELUInt32Min(uint32_t lhs, uint32_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

float MELDegreesToRadians(float degrees) {
    return degrees * MEL_PI / 180;
}

float MELRadiansToDegrees(float radians) {
    return radians * 180.0f / MEL_PI;
}

int MELRadiansToDegreesAsInteger(float radians) {
    return ((int)roundf(radians * 180.0f / MEL_PI) + 1080) % 360;
}

float MELDifferenceBetweenAngles(float lhs, float rhs) {
    const float difference = rhs - lhs;

    if (difference < -MEL_PI) {
        return difference + MEL_PI * 2;
    } else if (difference > MEL_PI) {
        return difference - MEL_PI * 2;
    } else {
        return difference;
    }
}

float MELFloatSquare(float value) {
    return value * value;
}

int MELIntSquare(int value) {
    return value * value;
}

GLfloat MELProgress(GLfloat from, GLfloat to, GLfloat value) {
    return fminf(fmaxf(value - from, 0) / (to - from), 1);
}

GLfloat MELEaseInOut(GLfloat from, GLfloat to, GLfloat value) {
    return MELFloatSquare(sinf(MEL_PI / 2 * MELProgress(from, to, value)));
}

GLfloat MELEaseIn(GLfloat from, GLfloat to, GLfloat value) {
    return MELFloatSquare(MELProgress(from, to, value));
}

GLfloat MELEaseInBack(GLfloat from, GLfloat to, GLfloat value) {
    const float x = MELProgress(from, to, value);
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return c3 * x * x * x - c1 * x * x;
}

GLfloat MELEaseOut(GLfloat from, GLfloat to, GLfloat value) {
    return 1.0f - MELFloatSquare(1.0f - MELProgress(from, to, value));
}

GLfloat MELEaseOutCubic(GLfloat from, GLfloat to, GLfloat value) {
    return 1 - powf(1 - MELProgress(from, to, value), 3);
}

GLfloat MELEaseOutBack(GLfloat from, GLfloat to, GLfloat value) {
    const float x = MELProgress(from, to, value);
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;
    return 1 + c3 * powf(x - 1, 3) + c1 * (x - 1) * (x - 1);
}

GLfloat MELEaseOutElastic(GLfloat from, GLfloat to, GLfloat value) {
    const float x = MELProgress(from, to, value);
    const float c4 = (2.0f * MEL_PI) / 3.0f;
    return x == 0.0f
      ? 0.0f
      : x == 1.0f
      ? 1.0f
      : powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
}

float MELEaseOutCirc(float from, float to, float value) {
    const float x = MELProgress(from, to, value);
    return sqrtf(1 - (x - 1) * (x - 1));
}
