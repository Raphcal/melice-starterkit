//
//  direction.c
//  Roll
//
//  Created by Raphaël Calabro on 22/03/2022.
//

#include "direction.h"

#include "melmath.h"

const GLfloat MELDirectionValues[DIRECTION_COUNT] = {-1.0f, 1.0f, -1.0f, 1.0f};
const GLfloat MELDirectionAngles[DIRECTION_COUNT] = {MEL_PI, 0.0f, MEL_PI * 1.5f, MEL_PI / 2.0f};
const MELDirection MELDirectionReverses[DIRECTION_COUNT] = {MELDirectionRight, MELDirectionLeft, MELDirectionDown, MELDirectionUp};
const LCDBitmapFlip MELDirectionFlip[DIRECTION_COUNT] = {kBitmapFlippedX, kBitmapUnflipped, kBitmapFlippedY, kBitmapUnflipped};
const MELAxe MELDirectionAxe[DIRECTION_COUNT] = {MELAxeHorizontal, MELAxeHorizontal, MELAxeVertical, MELAxeVertical};
const MELIntPoint MELDirectionIntPoint[DIRECTION_COUNT] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

MELBoolean MELDirectionIsSameValue(MELDirection self, GLfloat value) {
    return MELDirectionValues[self] * value >= 0.0f;
}
