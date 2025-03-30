//
//  direction.h
//  Roll
//
//  Created by Raphaël Calabro on 22/03/2022.
//

#ifndef direction_h
#define direction_h

#include "melstd.h"

#include "axe.h"
#include "point.h"

#define DIRECTION_COUNT 4

typedef enum meldirection {
    MELDirectionLeft,
    MELDirectionRight,
    MELDirectionUp,
    MELDirectionDown,
} MELDirection;

extern const GLfloat MELDirectionValues[DIRECTION_COUNT];
extern const GLfloat MELDirectionAngles[DIRECTION_COUNT];
extern const MELDirection MELDirectionReverses[DIRECTION_COUNT];
extern const LCDBitmapFlip MELDirectionFlip[DIRECTION_COUNT];
extern const MELAxe MELDirectionAxe[DIRECTION_COUNT];
extern const MELIntPoint MELDirectionIntPoint[DIRECTION_COUNT];

MELBoolean MELDirectionIsSameValue(MELDirection self, GLfloat value);

#endif /* direction_h */
