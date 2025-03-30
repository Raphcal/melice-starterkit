//
//  animationdirection.c
//  Roll
//
//  Created by Raphaël Calabro on 23/06/2022.
//

#include "animationdirection.h"

#include "melmath.h"

static MELAnimationDirection stepForAngle(GLfloat radians) {
    int degrees = MELRadiansToDegreesAsInteger(radians);
    switch (degrees) {
        case 90:
            return MELAnimationDirectionBottom;
        case 91 ... 135:
            return MELAnimationDirectionBottomLeft;
        case 136 ... 224:
            return MELAnimationDirectionLeft;
        case 270:
            return MELAnimationDirectionTop;
        case 316 ... 359:
            return MELAnimationDirectionRight;
        default:
            return (MELAnimationDirection) degrees / 45;
    }
}

MELAnimationDirectionFlip MELAnimationDirectionFlipForAngle(float angle, MELDirection direction) {
    MELAnimationDirection step = stepForAngle(angle);
    MELAnimationAimDirection aimAndDirection = (MELAnimationAimDirection) (step | (direction << 3));
    switch (aimAndDirection) {
        case MELAnimationAimLeftDirectionRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionRight, kBitmapFlippedY};
        case MELAnimationAimLeftDirectionBottomRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTopRight, kBitmapFlippedY};
        case MELAnimationAimLeftDirectionBottom:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTop, kBitmapFlippedY};
        case MELAnimationAimLeftDirectionBottomLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionBottomRight, kBitmapFlippedX};
        case MELAnimationAimLeftDirectionLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionRight, kBitmapFlippedX};
        case MELAnimationAimLeftDirectionTopLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTopRight, kBitmapFlippedX};
        case MELAnimationAimLeftDirectionTop:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTop, kBitmapFlippedX};
        case MELAnimationAimLeftDirectionTopRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionBottomRight, kBitmapFlippedY};

        case MELAnimationAimRightDirectionRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionRight, kBitmapUnflipped};
        case MELAnimationAimRightDirectionBottomRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionBottomRight, kBitmapUnflipped};
        case MELAnimationAimRightDirectionBottom:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTop, kBitmapFlippedXY};
        case MELAnimationAimRightDirectionBottomLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTopRight, kBitmapFlippedXY};
        case MELAnimationAimRightDirectionLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionRight, kBitmapFlippedXY};
        case MELAnimationAimRightDirectionTopLeft:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionBottomRight, kBitmapFlippedXY};
        case MELAnimationAimRightDirectionTop:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTop, kBitmapUnflipped};
        case MELAnimationAimRightDirectionTopRight:
            return (MELAnimationDirectionFlip) {MELAnimationDirectionTopRight, kBitmapUnflipped};
        default:
            playdate->system->error("Unsupported step, expected [0-14], but was: %d\n", (int) step);
            return (MELAnimationDirectionFlip) {MELAnimationDirectionRight, kBitmapUnflipped};
    }
}
