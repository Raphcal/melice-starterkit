//
//  size.h
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef size_h
#define size_h

#include "melstd.h"

typedef struct melsize {
    float width;
    float height;
} MELSize;

typedef struct melintsize {
    int32_t width;
    int32_t height;
} MELIntSize;

/**
 * A size whose width and height are both zero.
 */
extern const MELSize MELSizeZero;

/**
 * A size whose width and height are both zero.
 */
extern const MELIntSize MELIntSizeZero;

/**
 * Returns a size with the given width and height.
 *
 * @param width Width.
 * @param height Height.
 * @return A size.
 */
MELSize MELSizeMake(float width, float height);

/**
 * Returns a size with the given width and height.
 *
 * @param width Width.
 * @param height Height.
 * @return A size.
 */
MELIntSize MELIntSizeMake(int32_t width, int32_t height);

/**
 * Compares the two given sizes.
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return true if the sizes are equals, false otherwise.
 */
MELBoolean MELSizeEquals(MELSize lhs, MELSize rhs);
/**
 * Compares the two given sizes.
 *
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return true if the sizes are equals, false otherwise.
 */
MELBoolean MELIntSizeEquals(MELIntSize lhs, MELIntSize rhs);

MELSize MELSizeAdd(MELSize lhs, MELSize rhs);
MELIntSize MELIntSizeAdd(MELIntSize lhs, MELIntSize rhs);

MELSize MELSizeSubstract(MELSize lhs, MELSize rhs);
MELIntSize MELIntSizeSubstract(MELIntSize lhs, MELIntSize rhs);

MELSize MELSizeMultiply(MELSize lhs, MELSize rhs);
MELIntSize MELIntSizeMultiply(MELIntSize lhs, MELIntSize rhs);

MELSize MELSizeDivide(MELSize lhs, MELSize rhs);
MELIntSize MELIntSizeDivide(MELIntSize lhs, MELIntSize rhs);

MELSize MELSizeDivideByValue(MELSize lhs, float value);

#endif /* size_h */
