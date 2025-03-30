//
//  melmath.h
//  shmup
//
//  Created by Raphaël Calabro on 11/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef melmath_h
#define melmath_h

#include "melstd.h"

#define MEL_E    2.71828182845904523536028747135266250f  // e
#define MEL_2_PI 6.28318530717958647692528676655900576f  // 2 * π
#define MEL_PI   3.14159265358979323846264338327950288f  // π
#define MEL_PI_2 1.57079632679489661923132169163975144f  // π / 2
#define MEL_PI_4 0.785398163397448309615660845819875721f // π / 4

/**
 * @brief Returns the sign of a floating-point value.
 *
 * @param value The input value.
 * @return The sign of the input value: 1 if positive, -1 if negative, 0 if zero.
 */
int MELFloatSign(float value);

/**
 * @brief Returns the minimum value between two floating-point numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The minimum value between lhs and rhs.
 */
float MELFloatMin(float lhs, float rhs);

/**
 * @brief Returns the minimum value between two uint8_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The minimum value between lhs and rhs.
 */
uint8_t MELUInt8Min(uint8_t lhs, uint8_t rhs);

/**
 * @brief Returns the maximum value between two uint8_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
uint8_t MELUInt8Max(uint8_t lhs, uint8_t rhs);

/**
 * @brief Returns the minimum value among a variable number of double values.
 *
 * @param count Number of arguments.
 * @param ... Values.
 * @return The minimum value.
 */
double MELDoubleMinVariadic(int count, ...);

/**
 * @brief Returns the maximum value between two floating-point numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
float MELFloatMax(float lhs, float rhs);

/**
 * @brief Returns the maximum value between three floating-point numbers.
 *
 * @param a First value.
 * @param b Second value.
 * @param c Third value.
 * @return The maximum value among a, b, and c.
 */
float MELFloatMax3(float a, float b, float c);

/**
 * @brief Returns the maximum value among a variable number of double values.
 *
 * @param count Number of arguments.
 * @param ... Values.
 * @return The maximum value.
 */
double MELDoubleMaxVariadic(int count, ...);

/**
 * @brief Clamps a floating-point value within a specified range.
 *
 * Returns the given value if it is greater than min and less than max.
 * Otherwise, min will be returned if the value is less than min or else max.
 *
 * @param min Minimum value.
 * @param value Value to clamp.
 * @param max Maximum value.
 * @return The clamped value.
 */
float MELFloatBound(float min, float value, float max);

/**
 * @brief Clamps an integer value within a specified range.
 *
 * Returns the given value if it is greater than min and less than max.
 * Otherwise, min will be returned if the value is less than min or else max.
 *
 * @param min Minimum value.
 * @param value Value to clamp.
 * @param max Maximum value.
 * @return The clamped value.
 */
int32_t MELIntBound(int32_t min, int32_t value, int32_t max);

/**
 * @brief Returns the maximum value between two integers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
int MELIntMax(int lhs, int rhs);

/**
 * @brief Returns the maximum value between two int32_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
int32_t MELInt32Max(int32_t lhs, int32_t rhs);

/**
 * @brief Returns the maximum value between two uint32_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
uint32_t MELUInt32Max(uint32_t lhs, uint32_t rhs);

/**
 * @brief Returns the maximum value between two int64_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The maximum value between lhs and rhs.
 */
int64_t MELInt64Max(int64_t lhs, int64_t rhs);

/**
 * @brief Returns the minimum value between two integers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The minimum value between lhs and rhs.
 */
int MELIntMin(int lhs, int rhs);

/**
 * @brief Returns the minimum value between two uint32_t numbers.
 *
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The minimum value between lhs and rhs.
 */
uint32_t MELUInt32Min(uint32_t lhs, uint32_t rhs);

/**
 * @brief Converts degrees to radians.
 *
 * @param degrees Angle in degrees.
 * @return The angle in radians.
 */
float MELDegreesToRadians(float degrees);

/**
 * @brief Converts radians to degrees.
 *
 * @param radians Angle in radians.
 * @return The angle in degrees.
 */
float MELRadiansToDegrees(float radians);

/**
 * @brief Converts radians to degrees as an integer.
 *
 * @param radians Angle in radians.
 * @return The angle in degrees as an integer.
 */
int MELRadiansToDegreesAsInteger(float radians);

/**
 * @brief Calculates the absolute difference between two angles in radians.
 *
 * @param lhs The first angle.
 * @param rhs The second angle.
 * @return The absolute difference between lhs and rhs.
 */
float MELDifferenceBetweenAngles(float lhs, float rhs);

/**
 * @brief Returns the square of a floating-point value.
 *
 * @param value The input value.
 * @return The square of the input value.
 */
float MELFloatSquare(float value);

/**
 * @brief Returns the square of an integer value.
 *
 * @param value The input value.
 * @return The square of the input value.
 */
int MELIntSquare(int value);

/**
 * @brief Maps a value from one range to another.
 *
 * @param from The starting value of the range.
 * @param to The ending value of the range.
 * @param value The value to map.
 * @return The mapped value.
 */
float MELProgress(float from, float to, float value);

/**
 * @brief Computes the ease-in-out interpolation between two values.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseInOut(float from, float to, float value);

/**
 * @brief Computes the ease-in interpolation between two values.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseIn(float from, float to, float value);

/**
 * @brief Computes the ease-in interpolation with overshooting effect.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseInBack(float from, float to, float value);

/**
 * @brief Computes the ease-out interpolation between two values.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseOut(float from, float to, float value);

/**
 * @brief Computes the ease-out cubic interpolation between two values.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseOutCubic(float from, float to, float value);

/**
 * @brief Computes the ease-out interpolation with overshooting effect.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseOutBack(float from, float to, float value);

/**
 * @brief Computes the ease-out interpolation with elastic effect.
 *
 * @param from The starting value.
 * @param to The ending value.
 * @param value The interpolation factor.
 * @return The interpolated value.
 */
float MELEaseOutElastic(float from, float to, float value);

float MELEaseOutCirc(float from, float to, float value);

#endif /* melmath_h */
