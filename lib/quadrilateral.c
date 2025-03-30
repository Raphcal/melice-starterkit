//
//  quadrilateral.c
//  shmup
//
//  Created by Raphaël Calabro on 17/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "quadrilateral.h"

#include "rectangle.h"
#include "melmath.h"

MELQuadrilateral MELQuadrilateralMake(MELPoint topLeft, MELPoint topRight, MELPoint bottomLeft, MELPoint bottomRight) {
    return (MELQuadrilateral) {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight
    };
}

float MELQuadrilateralGetLeft(MELQuadrilateral self) {
    return MELFloatMin(self.topLeft.x, MELFloatMin(self.topRight.x, MELFloatMin(self.bottomLeft.x, self.bottomRight.x)));
}

float MELQuadrilateralGetRight(MELQuadrilateral self) {
    return MELFloatMax(self.topLeft.x, MELFloatMax(self.topRight.x, MELFloatMax(self.bottomLeft.x, self.bottomRight.x)));
}

float MELQuadrilateralGetTop(MELQuadrilateral self) {
    return MELFloatMin(self.topLeft.y, MELFloatMin(self.topRight.y, MELFloatMin(self.bottomLeft.y, self.bottomRight.y)));
}

float MELQuadrilateralGetBottom(MELQuadrilateral self) {
    return MELFloatMax(self.topLeft.y, MELFloatMax(self.topRight.y, MELFloatMax(self.bottomLeft.y, self.bottomRight.y)));
}

MELRectangle MELQuadrilateralGetEnclosingRectangle(MELQuadrilateral self) {
    const float left = MELQuadrilateralGetLeft(self);
    const float right = MELQuadrilateralGetRight(self);
    const float top = MELQuadrilateralGetTop(self);
    const float bottom = MELQuadrilateralGetBottom(self);
    return MELRectangleMake(left, top, right - left, bottom - top);
}
