//
//  point.c
//  shmup
//
//  Created by Raphaël Calabro on 24/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "point.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "size.h"
#include "direction.h"

const MELPoint MELPointZero = {0, 0};
const MELIntPoint MELIntPointZero = {0, 0};

MELPoint MELPointMake(float x, float y) {
    return (MELPoint) {
        x,
        y
    };
}

MELIntPoint MELIntPointMake(int32_t x, int32_t y) {
    return (MELIntPoint) {
        x,
        y
    };
}

MELBoolean MELPointEquals(MELPoint lhs, MELPoint rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

MELBoolean MELIntPointEquals(MELIntPoint lhs, MELIntPoint rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

MELPoint MELPointAdd(MELPoint lhs, MELPoint rhs) {
    return (MELPoint) {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}

MELIntPoint MELIntPointAdd(MELIntPoint lhs, MELIntPoint rhs) {
    return (MELIntPoint) {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}

MELPoint MELPointAddSize(MELPoint point, MELSize size) {
    return (MELPoint) {
        point.x + size.width,
        point.y + size.height
    };
}

MELPoint MELPointSubstract(MELPoint lhs, MELPoint rhs) {
    return (MELPoint) {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}

MELIntPoint MELIntPointSubstract(MELIntPoint lhs, MELIntPoint rhs) {
    return (MELIntPoint) {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}

MELPoint MELPointSubstractSize(MELPoint point, MELSize size) {
    return (MELPoint) {
        point.x - size.width,
        point.y - size.height
    };
}

MELPoint MELPointMultiply(MELPoint lhs, MELPoint rhs) {
    return (MELPoint) {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}

MELIntPoint MELIntPointMultiply(MELIntPoint lhs, MELIntPoint rhs) {
    return (MELIntPoint) {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}

MELPoint MELPointMultiplyByValue(MELPoint self, float value) {
    return (MELPoint) {
        self.x * value,
        self.y * value
    };
}

MELPoint MELPointMultiplyByDirection(MELPoint self, MELDirection direction) {
    switch (direction) {
        case MELDirectionLeft:
            return (MELPoint) {
                // NOTE: May be better to return -fabsf(self.x) here instead and fabsf(self.x) for right direction.
                .x = -self.x,
                .y = self.y
            };
        case MELDirectionUp:
            return (MELPoint) {
                .x = self.x,
                .y = -self.y
            };
        default:
            return self;
    }
}

MELPoint MELPointDivide(MELPoint lhs, MELPoint rhs) {
    return (MELPoint) {
        rhs.x != 0 ? lhs.x / rhs.x : 0,
        rhs.y != 0 ? lhs.y / rhs.y : 0
    };
}

MELPoint MELPointDivideByValue(MELPoint self, GLfloat value) {
    if (value == 0) {
        return MELPointZero;
    } else {
        return (MELPoint) {
            self.x / value,
            self.y / value
        };
    }
}

MELIntPoint MELIntPointDivide(MELIntPoint lhs, MELIntPoint rhs) {
    return (MELIntPoint) {
        rhs.x != 0 ? lhs.x / rhs.x : 0,
        rhs.y != 0 ? lhs.y / rhs.y : 0
    };
}
MELIntPoint MELIntPointDivideBySize(MELIntPoint lhs, MELIntSize rhs) {
    return (MELIntPoint) {
        lhs.x / rhs.width,
        lhs.y / rhs.height
    };
}

float MELPointDistanceToPoint(MELPoint self, MELPoint other) {
    return hypotf(self.x - other.x, self.y - other.y);
}

float MELPointAngleToPoint(MELPoint self, MELPoint other) {
    return atan2f(self.y - other.y, self.x - other.x);
}
