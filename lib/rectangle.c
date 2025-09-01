//
//  rectangle.c
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "rectangle.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "quadrilateral.h"
#include "list.h"

const MELRectangle MELRectangleZero = { {0, 0}, {0, 0} };
const MELIntRectangle MELIntRectangleZero = { {0, 0}, {0, 0} };

MELRectangle MELRectangleMake(float x, float y, float width, float height) {
    return (MELRectangle) {
        {
            x,
            y
        },
        {
            width,
            height
        }
    };
}

MELRectangle MELRectangleMakeWithOriginAndSize(MELPoint origin, MELSize size) {
    return (MELRectangle) {
        origin,
        size
    };
}

MELRectangle MELRectangleMakeWithPoints(MELPoint a, MELPoint b) {
    MELRectangle self;
    if (a.x < b.x) {
        self.origin.x = a.x;
        self.size.width = b.x - a.x;
    } else {
        self.origin.x = b.x;
        self.size.width = a.x - b.x;
    }
    if (a.y < b.y) {
        self.origin.y = a.y;
        self.size.height = b.y - a.y;
    } else {
        self.origin.y = b.y;
        self.size.height = a.y - b.y;
    }
    return self;
}

MELIntRectangle MELIntRectangleMake(int32_t x, int32_t y, int32_t width, int32_t height) {
    return (MELIntRectangle) {
        {
            x,
            y
        },
        {
            width,
            height
        }
    };
}

MELIntRectangle MELIntRectangleMakeWithOriginAndSize(MELIntPoint origin, MELIntSize size) {
    return (MELIntRectangle) {
        origin,
        size
    };
}

MELIntRectangle MELIntRectangleMakeWithPoints(MELIntPoint a, MELIntPoint b) {
    MELIntRectangle self;
    if (a.x < b.x) {
        self.origin.x = a.x;
        self.size.width = b.x - a.x;
    } else {
        self.origin.x = b.x;
        self.size.width = a.x - b.x;
    }
    if (a.y < b.y) {
        self.origin.y = a.y;
        self.size.height = b.y - a.y;
    } else {
        self.origin.y = b.y;
        self.size.height = a.y - b.y;
    }
    return self;
}

float MELRectangleOriginIsCenterGetLeft(MELRectangle self) {
    return self.origin.x - self.size.width / 2.0f;
}

void MELRectangleOriginIsCenterSetLeft(MELRectangle * _Nonnull self, float left) {
    self->origin.x = left + self->size.width / 2.0f;
}

float MELRectangleOriginIsCenterGetRight(MELRectangle self) {
    return self.origin.x + self.size.width / 2.0f;
}

void MELRectangleOriginIsCenterSetRight(MELRectangle * _Nonnull self, float right) {
    self->origin.x = right - self->size.width / 2.0f;
}

float MELRectangleOriginIsCenterGetTop(MELRectangle self) {
    return self.origin.y - self.size.height / 2.0f;
}

void MELRectangleOriginIsCenterSetTop(MELRectangle * _Nonnull self, float top) {
    self->origin.y = top + self->size.height / 2.0f;
}

float MELRectangleOriginIsCenterGetBottom(MELRectangle self) {
    return self.origin.y + self.size.height / 2.0f;
}

void MELRectangleOriginIsCenterSetBottom(MELRectangle * _Nonnull self, float bottom) {
    self->origin.y = bottom - self->size.height / 2.0f;
}

MELPoint MELRectangleOriginIsCenterGetTopLeft(MELRectangle self) {
    return (MELPoint) {
        .x = MELRectangleOriginIsCenterGetLeft(self),
        .y = MELRectangleOriginIsCenterGetTop(self),
    };
}

MELPoint MELRectangleOriginIsCenterGetPointAtAngle(MELRectangle self, GLfloat angle) {
    const GLfloat halfHeight = self.size.height / 2.0f;
    return MELPointMake(self.origin.x + halfHeight * cosf(angle), self.origin.y + halfHeight * sinf(angle));
}

MELBoolean MELRectangleEquals(MELRectangle lhs, MELRectangle rhs) {
    return lhs.origin.x == rhs.origin.x &&
        lhs.origin.y == rhs.origin.y &&
        lhs.size.width == rhs.size.width &&
        lhs.size.height == rhs.size.height;
}

MELBoolean MELIntRectangleEquals(MELIntRectangle lhs, MELIntRectangle rhs) {
    return lhs.origin.x == rhs.origin.x &&
        lhs.origin.y == rhs.origin.y &&
        lhs.size.width == rhs.size.width &&
        lhs.size.height == rhs.size.height;
}

MELQuadrilateral MELRectangleRotate(MELRectangle self, float rotation) {
    return MELRectangleRotateWithPivot(self, rotation, self.origin);
}

MELQuadrilateral MELRectangleRotateWithPivot(MELRectangle self, float rotation, MELPoint pivot) {
    MELPoint vertices[4];
    const float left = MELRectangleOriginIsCenterGetLeft(self);
    const float right = MELRectangleOriginIsCenterGetRight(self);
    const float top = MELRectangleOriginIsCenterGetTop(self);
    const float bottom = MELRectangleOriginIsCenterGetBottom(self);
    MELPoint source[4] = {
        MELPointMake(left, top),
        MELPointMake(right, top),
        MELPointMake(left, bottom),
        MELPointMake(right, bottom)
    };
    for (int index = 0; index < 4; index++) {
        const MELPoint vertex = source[index];
        const float length = MELPointDistanceToPoint(vertex, pivot);
        const float angle = MELPointAngleToPoint(vertex, pivot);
        vertices[index] = MELPointMake(pivot.x + cosf(angle + rotation) * length, pivot.y + sinf(angle + rotation) * length);
    }
    return MELQuadrilateralMake(vertices[0], vertices[1], vertices[2], vertices[3]);
}

MELBoolean MELRectangleOriginIsTopLeftContainsPoint(const MELRectangle self, const MELPoint point) {
    return point.x >= self.origin.x &&
        point.x < self.origin.x + self.size.width &&
        point.y >= self.origin.y &&
        point.y < self.origin.y + self.size.height;
}
MELBoolean MELIntRectangleOriginIsTopLeftContainsPoint(const MELIntRectangle self, const MELIntPoint point) {
    return point.x >= self.origin.x &&
        point.x < self.origin.x + self.size.width &&
        point.y >= self.origin.y &&
        point.y < self.origin.y + self.size.height;
}

MELBoolean MELRectangleIntersectsWithRectangle(const MELRectangle self, const MELRectangle other) {
    const MELBoolean x = fabsf(self.origin.x - other.origin.x) <= (self.size.width + other.size.width) / 2;
    const MELBoolean y = fabsf(self.origin.y - other.origin.y) <= (self.size.height + other.size.height) / 2;
    return x && y;
}
MELBoolean MELIntRectangleIntersectsWithRectangle(const MELIntRectangle self, const MELIntRectangle other) {
    const MELBoolean x = abs(self.origin.x - other.origin.x) <= (self.size.width + other.size.width) / 2;
    const MELBoolean y = abs(self.origin.y - other.origin.y) <= (self.size.height + other.size.height) / 2;
    return x && y;
}

MELBoolean MELRectangleOriginIsTopLeftContainsRectangle(MELRectangle self, MELRectangle other) {
    return
    // Gauche
    other.origin.x >= self.origin.x
    // Droite
    && (other.origin.x + other.size.width <= self.origin.x + self.size.width)
    // Haut
    && other.origin.y >= self.origin.y
    // Droite
    && (other.origin.y + other.size.height <= self.origin.y + self.size.height);
}

MELBoolean MELIntRectangleOriginIsTopLeftContainsRectangle(MELIntRectangle self, MELIntRectangle other) {
    return
    // Gauche
    other.origin.x >= self.origin.x
    // Droite
    && (other.origin.x + other.size.width <= self.origin.x + self.size.width)
    // Haut
    && other.origin.y >= self.origin.y
    // Droite
    && (other.origin.y + other.size.height <= self.origin.y + self.size.height);
}

static float locationBetweenPoints(float a1, float a2, float b1, float b2, float value) {
    return a1 + (a2 - a1) * (value - b1) / (b2 - b1);
}

MELBoolean MELRectangleIntersectsWithSegment(const MELRectangle self, const MELSegment segment) {
    MELPoint segmentLeft, segmentRight, segmentTop, segmentBottom;
    if (segment.from.x < segment.to.x) {
        segmentLeft = segment.from;
        segmentRight = segment.to;
    } else {
        segmentLeft = segment.to;
        segmentRight = segment.from;
    }
    if (segment.from.y < segment.to.y) {
        segmentTop = segment.from;
        segmentBottom = segment.to;
    } else {
        segmentTop = segment.to;
        segmentBottom = segment.from;
    }
    const float top = MELRectangleOriginIsCenterGetTop(self);
    const float bottom = MELRectangleOriginIsCenterGetBottom(self);
    const float left = MELRectangleOriginIsCenterGetLeft(self);
    const float right = MELRectangleOriginIsCenterGetRight(self);

    const float xOnRectangleTop = locationBetweenPoints(segmentLeft.x, segmentRight.x, segmentLeft.y, segmentRight.y, top);
    const float xOnRectangleBottom = locationBetweenPoints(segmentLeft.x, segmentRight.x, segmentLeft.y, segmentRight.y, bottom);
    
    const float yOnRectangleLeft = locationBetweenPoints(segmentTop.y, segmentBottom.y, segmentTop.x, segmentBottom.x, left);
    const float yOnRectangleRight = locationBetweenPoints(segmentTop.y, segmentBottom.y, segmentTop.x, segmentBottom.x, right);

    // NOTE: Pour les dessins, simplement dessiner le triangle en transparent :)

    return (xOnRectangleTop >= left && xOnRectangleTop <= right)
    || (xOnRectangleBottom >= left && xOnRectangleBottom <= right)
    || (yOnRectangleLeft >= top && yOnRectangleLeft <= bottom)
    || (yOnRectangleRight >= top && yOnRectangleRight <= bottom);
}
