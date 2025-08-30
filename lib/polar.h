//
//  polar.h
//  relaxe
//
//  Created by Raphaël Calabro on 06/05/2025.
//

#ifndef polar_h
#define polar_h

#include "melstd.h"

#include "point.h"

typedef struct {
    float distance;
    float angle;
} MELPolarPoint;

MELPoint MELPolarPointToPoint(MELPolarPoint self, MELPoint center, float angle);

#endif /* polar_h */
