//
//  size.c
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "size.h"

const MELSize MELSizeZero = {0, 0};
const MELIntSize MELIntSizeZero = {0, 0};

MELSize MELSizeMake(float width, float height) {
    return (MELSize) {
        width,
        height
    };
}

MELIntSize MELIntSizeMake(int32_t width, int32_t height) {
    return (MELIntSize) {
        width,
        height
    };
}

MELBoolean MELSizeEquals(MELSize lhs, MELSize rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

MELBoolean MELIntSizeEquals(MELIntSize lhs, MELIntSize rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

MELSize MELSizeAdd(MELSize lhs, MELSize rhs) {
    return (MELSize) {lhs.width + rhs.width, lhs.height + rhs.height};
}

MELIntSize MELIntSizeAdd(MELIntSize lhs, MELIntSize rhs) {
    return (MELIntSize) {lhs.width + rhs.width, lhs.height + rhs.height};
}

MELSize MELSizeSubstract(MELSize lhs, MELSize rhs) {
    return (MELSize) {lhs.width - rhs.width, lhs.height - rhs.height};
}

MELIntSize MELIntSizeSubstract(MELIntSize lhs, MELIntSize rhs) {
    return (MELIntSize) {lhs.width - rhs.width, lhs.height - rhs.height};
}

MELSize MELSizeMultiply(MELSize lhs, MELSize rhs) {
    return (MELSize) {lhs.width * rhs.width, lhs.height * rhs.height};
}

MELIntSize MELIntSizeMultiply(MELIntSize lhs, MELIntSize rhs) {
    return (MELIntSize) {lhs.width * rhs.width, lhs.height * rhs.height};
}

MELSize MELSizeDivide(MELSize lhs, MELSize rhs) {
    return (MELSize) {lhs.width / rhs.width, lhs.height / rhs.height};
}

MELIntSize MELIntSizeDivide(MELIntSize lhs, MELIntSize rhs) {
    return (MELIntSize) {lhs.width / rhs.width, lhs.height / rhs.height};
}

MELSize MELSizeDivideByValue(MELSize lhs, float value) {
    return (MELSize) {lhs.width / value, lhs.height / value};
}
