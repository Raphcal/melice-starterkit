//
//  rectangle.h
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef rectangle_h
#define rectangle_h

#include "melstd.h"

#include "point.h"
#include "size.h"

typedef struct melquadrilateral MELQuadrilateral;

/**
 * A rectangle.
 */
typedef struct melrectangle {
    MELPoint origin;
    MELSize size;
} MELRectangle;

/**
 * A rectangle with integer coordinates.
 */
typedef struct {
    MELIntPoint origin;
    MELIntSize size;
} MELIntRectangle;

/**
 * A rectangle whose origin and size are both zero.
 */
extern const MELRectangle MELRectangleZero;

/**
 * A rectangle whose origin and size are both zero.
 */
extern const MELIntRectangle MELIntRectangleZero;

/**
 * Returns a rectangle with the given origin and size.
 *
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @return A rectangle.
 */
MELRectangle MELRectangleMake(float x, float y, float width, float height);

/**
 * Returns a rectangle with the given origin and size.
 *
 * @param origin Rectangle origin. Maybe the center or the top left point depending on the context.
 * @param size Rectangle size.
 * @return A rectangle.
 */
MELRectangle MELRectangleMakeWithOriginAndSize(MELPoint origin, MELSize size);

/**
 * Returns a rectangle whose origin is top-left based and the width and height are calculated by the given points.
 *
 * @param a First point defining the rectangle.
 * @param b Second point defining the rectangle.
 * @return A rectangle.
 */
MELRectangle MELRectangleMakeWithPoints(MELPoint a, MELPoint b);

/**
 * Returns a rectangle with the given origin and size.
 *
 * @param x Horizontal coordinate.
 * @param y Vertical coordinate.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @return A rectangle.
 */
MELIntRectangle MELIntRectangleMake(int32_t x, int32_t y, int32_t width, int32_t height);

/**
 * Returns a rectangle with the given origin and size.
 *
 * @param origin Rectangle origin. Maybe the center or the top left point depending on the context.
 * @param size Rectangle size.
 * @return A rectangle.
 */
MELIntRectangle MELIntRectangleMakeWithOriginAndSize(MELIntPoint origin, MELIntSize size);

/**
 * Returns a rectangle whose origin is top-left based and the width and height are calculated by the given points.
 *
 * @param a First point defining the rectangle.
 * @param b Second point defining the rectangle.
 * @return A rectangle.
 */
MELIntRectangle MELIntRectangleMakeWithPoints(MELIntPoint a, MELIntPoint b);

/**
 * Returns the minimum X coordinate of a rectangle whose center is its origin.
 *
 * @param self Rectangle.
 * @return The left coordinate of the given rectangle.
 */
float MELRectangleOriginIsCenterGetLeft(MELRectangle self);

/**
 * Sets the X coordinate.
 *
 * @param self Rectangle.
 * @param left Left X coordinate of the rectangle.
 */
void MELRectangleOriginIsCenterSetLeft(MELRectangle * _Nonnull self, float left);

/**
 * Returns the maximum X coordinate of a rectangle whose center is its origin.
 *
 * @param self Rectangle.
 * @return The right coordinate of the given rectangle.
 */
float MELRectangleOriginIsCenterGetRight(MELRectangle self);

/**
 * Sets the X coordinate.
 *
 * @param self Rectangle.
 * @param right Right X coordinate of the rectangle.
 */
void MELRectangleOriginIsCenterSetRight(MELRectangle * _Nonnull self, float right);

/**
 * Returns the minimum Y coordinate of a rectangle whose center is its origin.
 *
 * @param self Rectangle.
 * @return The top coordinate of the given rectangle.
 */
float MELRectangleOriginIsCenterGetTop(MELRectangle self);

/**
 * Sets the Y coordinate.
 *
 * @param self Rectangle.
 * @param top Upper Y coordinate of the rectangle.
 */
void MELRectangleOriginIsCenterSetTop(MELRectangle * _Nonnull self, float top);

/**
 * Returns the maximum Y coordinate of a rectangle whose center is its origin.
 *
 * @param self Rectangle.
 * @return The bottom coordinate of the given rectangle.
 */
float MELRectangleOriginIsCenterGetBottom(MELRectangle self);

/**
 * Sets the Y coordinate.
 *
 * @param self Rectangle.
 * @param bottom Lower Y coordinate of the rectangle.
 */
void MELRectangleOriginIsCenterSetBottom(MELRectangle * _Nonnull self, float bottom);

MELPoint MELRectangleOriginIsCenterGetPointAtAngle(MELRectangle self, GLfloat angle);

MELBoolean MELRectangleEquals(MELRectangle lhs, MELRectangle rhs);
MELBoolean MELIntRectangleEquals(MELIntRectangle lhs, MELIntRectangle rhs);

MELQuadrilateral MELRectangleRotate(MELRectangle self, float rotation);

MELQuadrilateral MELRectangleRotateWithPivot(MELRectangle self, float rotation, MELPoint pivot);

MELBoolean MELRectangleOriginIsTopLeftContainsPoint(const MELRectangle self, const MELPoint point);
MELBoolean MELIntRectangleOriginIsTopLeftContainsPoint(const MELIntRectangle self, const MELIntPoint point);

MELBoolean MELRectangleIntersectsWithRectangle(const MELRectangle self, const MELRectangle other);
MELBoolean MELIntRectangleIntersectsWithRectangle(const MELIntRectangle self, const MELIntRectangle other);

MELBoolean MELIntRectangleOriginIsTopLeftGetIntersection(const MELIntRectangle self, const MELIntRectangle other, MELIntRectangle * _Nonnull intersection);

MELBoolean MELRectangleOriginIsTopLeftContainsRectangle(MELRectangle self, MELRectangle other);
MELBoolean MELIntRectangleOriginIsTopLeftContainsRectangle(MELIntRectangle self, MELIntRectangle other);

#endif /* rectangle_h */
