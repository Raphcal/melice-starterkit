//
//  polar.c
//  relaxe
//
//  Created by Raphaël Calabro on 06/05/2025.
//

#include "polar.h"

#include "melmath.h"

MELPoint MELPolarPointToPoint(MELPolarPoint self, MELPoint center, float angle) {
    const float radian = angle + MELDegreesToRadians(self.angle);
    return (MELPoint) {
        .x = center.x + cosf(radian) * self.distance,
        .y = center.y + sinf(radian) * self.distance,
    };
}
