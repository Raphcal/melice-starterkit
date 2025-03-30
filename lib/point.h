//
//  point.h
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef point_h
#define point_h

#include "melstd.h"

typedef struct melsize MELSize;
typedef struct melintsize MELIntSize;
typedef enum meldirection MELDirection;

/**
 * Floating coordinates of a point in a 2D plane.
 */
typedef struct {
    float x;
    float y;
} MELPoint;

/**
 * Integer coordinates of a point in a 2D plane.
 */
typedef struct {
    int32_t x;
    int32_t y;
} MELIntPoint;

/**
 * A point whose coordinates are both zero.
 */
extern const MELPoint MELPointZero;

/**
 * A point whose coordinates are both zero.
 */
extern const MELIntPoint MELIntPointZero;

/**
 * Returns a point with the given coordinates.
 *
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 * @return A point.
 */
MELPoint MELPointMake(float x, float y);

/**
 * Returns a point with the given coordinates.
 *
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 * @return A point.
 */
MELIntPoint MELIntPointMake(int32_t x, int32_t y);

/**
 * Compares the two given point.
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return true if the points are the same, false otherwise.
 */
MELBoolean MELPointEquals(MELPoint lhs, MELPoint rhs);

/**
 * Compares the two given point.
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return true if the points are the same, false otherwise.
 */
MELBoolean MELIntPointEquals(MELIntPoint lhs, MELIntPoint rhs);

/**
 * Adds the given points.
 *
 * @return A point whose coordinates are lhs.x + rhs.x and lhs.y + rhs.y.
 */
MELPoint MELPointAdd(MELPoint lhs, MELPoint rhs);

/**
 * Adds the given points.
 *
 * @return A point whose coordinates are lhs.x + rhs.x and lhs.y + rhs.y.
 */
MELIntPoint MELIntPointAdd(MELIntPoint lhs, MELIntPoint rhs);

/**
 * Adds the given points.
 *
 * @return A point whose coordinates are `point.x + size.width` and `point.y + size.height`.
 */
MELPoint MELPointAddSize(MELPoint point, MELSize size);

/**
 * Substracts the given points.
 *
 * @return A point whose coordinates are lhs.x - rhs.x and lhs.y - rhs.y.
 */
MELPoint MELPointSubstract(MELPoint lhs, MELPoint rhs);

/**
 * Substracts the given points.
 *
 * @return A point whose coordinates are lhs.x - rhs.x and lhs.y - rhs.y.
 */
MELIntPoint MELIntPointSubstract(MELIntPoint lhs, MELIntPoint rhs);

/**
 * Substracts the given size to the given point.
 *
 * @return A point whose coordinates are `point.x - size.width` and `point.y - size.height`.
 */
MELPoint MELPointSubstractSize(MELPoint point, MELSize size);

/**
 * Multiplies the given points.
 *
 * @return A point whose coordinates are lhs.x * rhs.x and lhs.y * rhs.y.
 */
MELPoint MELPointMultiply(MELPoint lhs, MELPoint rhs);

/**
 * Multiplies the given points.
 *
 * @return A point whose coordinates are lhs.x * rhs.x and lhs.y * rhs.y.
 */
MELIntPoint MELIntPointMultiply(MELIntPoint lhs, MELIntPoint rhs);

/**
 * Multiplies the coordinates of the given points by the given value.
 *
 * @return A point whose coordinates are self.x * value and self.y * value.
 */
MELPoint MELPointMultiplyByValue(MELPoint self, float value);

/**
 * Multiplies the x or y coordinate of the given point by the value of the given direction.
 * <ul>
 * <li> When direction is <code>MELDirectionLeft</code>, the <code>x</code> coordinate is multiplied by -1.</li>
 * <li> When direction is <code>MELDirectionUp</code>, the <code>y</code> coordinate is multiplied by -1.</li>
 * </ul>
 *
 * @return A point with x or y coordinate multiplied by the value of the given direction.
 */
MELPoint MELPointMultiplyByDirection(MELPoint self, MELDirection direction);

/**
 * Divides the coordinates of <code>lhs</code> points by those of <code>rhs</code>.
 * If a coordinate of rhs is 0, then the returned value will be 0 for this coordinate.
 *
 * @return A point whose coordinates are lhs.x / rhs.x and lhs.y / rhs.y.
 */
MELPoint MELPointDivide(MELPoint lhs, MELPoint rhs);

/**
 * Divides the coordinates of the given points by the given value.
 * If value is 0, the returned point will be (0, 0).
 *
 * @return A point whose coordinates are self.x / value and self.y / value.
 */
MELPoint MELPointDivideByValue(MELPoint self, GLfloat value);

MELIntPoint MELIntPointDivide(MELIntPoint lhs, MELIntPoint rhs);
MELIntPoint MELIntPointDivideBySize(MELIntPoint lhs, MELIntSize rhs);

/**
 * Returns the distance between the 2 points.
 *
 * @param self Source point.
 * @param other Other point.
 * @return The distance between the points.
 */
float MELPointDistanceToPoint(MELPoint self, MELPoint other);

/**
 * Returns the angle between the 2 points.
 *
 * @param self Source point.
 * @param other Other point.
 * @return The angle between the points.
 */
float MELPointAngleToPoint(MELPoint self, MELPoint other);

#endif /* point_h */
