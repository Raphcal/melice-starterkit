//
//  animationdirection.h
//  Roll
//
//  Created by Raphaël Calabro on 27/03/2022.
//

#ifndef animationdirection_h
#define animationdirection_h

#define MELAnimationDirectionCount 8

#include "melstd.h"

#include "direction.h"

typedef enum melanimationdirection {
    MELAnimationDirectionRight,
    MELAnimationDirectionBottomRight,
    MELAnimationDirectionBottom,
    MELAnimationDirectionBottomLeft,
    MELAnimationDirectionLeft,
    MELAnimationDirectionTopLeft,
    MELAnimationDirectionTop,
    MELAnimationDirectionTopRight,
} MELAnimationDirection;

typedef struct {
    MELAnimationDirection direction;
    LCDBitmapFlip flip;
} MELAnimationDirectionFlip;

typedef enum {
    MELAnimationAimLeftDirectionRight,
    MELAnimationAimLeftDirectionBottomRight,
    MELAnimationAimLeftDirectionBottom,
    MELAnimationAimLeftDirectionBottomLeft,
    MELAnimationAimLeftDirectionLeft,
    MELAnimationAimLeftDirectionTopLeft,
    MELAnimationAimLeftDirectionTop,
    MELAnimationAimLeftDirectionTopRight,

    MELAnimationAimRightDirectionRight,
    MELAnimationAimRightDirectionBottomRight,
    MELAnimationAimRightDirectionBottom,
    MELAnimationAimRightDirectionBottomLeft,
    MELAnimationAimRightDirectionLeft,
    MELAnimationAimRightDirectionTopLeft,
    MELAnimationAimRightDirectionTop,
    MELAnimationAimRightDirectionTopRight,
} MELAnimationAimDirection;

MELAnimationDirectionFlip MELAnimationDirectionFlipForAngle(float angle, MELDirection direction);

#endif /* animationdirection_h */
