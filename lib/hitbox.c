//
//  hitbox.c
//  shmup
//
//  Created by Raphaël Calabro on 27/02/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "hitbox.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "hitboxtype.h"
#include "statichitbox.h"
#include "spritehitbox.h"
#include "simplespritehitbox.h"

MELHitbox * _Nullable MELHitboxLoad(MELInputStream * _Nonnull inputStream, MELSprite * _Nonnull sprite) {
    MELHitboxType type = MELInputStreamReadByte(inputStream);
    switch (type) {
        case MELHitboxTypeStaticHitbox:
            return MELStaticHitboxLoad(inputStream);
        case MELHitboxTypeSpriteHitbox:
            return MELSpriteHitboxAlloc(sprite);
        case MELHitboxTypeSimpleSpriteHitbox:
            return MELSimpleSpriteHitboxAlloc(sprite);
        default:
            playdate->system->error("Unsupported hitbox type: %d", type);
            return NULL;
    }
}

void MELHitboxDeinit(MELHitbox * _Nullable self) {
    if (self != NULL && self->class->deinit != NULL) {
        self->class->deinit(self);
    }
}

void MELHitboxReaffect(MELHitbox * _Nonnull self, MELSprite * _Nonnull sprite) {
    if (self->class == &MELSpriteHitboxClass) {
        MELSpriteHitbox *spriteHitbox = (MELSpriteHitbox *)self;
        spriteHitbox->sprite = sprite;
    } else if (self->class == &MELSimpleSpriteHitboxClass) {
        MELSimpleSpriteHitbox *simpleSpriteHitbox = (MELSimpleSpriteHitbox *)self;
        simpleSpriteHitbox->sprite = sprite;
    }
}

MELRectangle MELHitboxGetFrame(MELHitbox * _Nonnull self) {
    return self->class->getFrame(self);
}

MELBoolean MELHitboxCollidesWithPoint(MELHitbox * _Nonnull self, MELPoint point) {
    const MELRectangle frame = self->class->getFrame(self);
    return point.x >= MELRectangleOriginIsCenterGetLeft(frame) &&
        point.x < MELRectangleOriginIsCenterGetRight(frame) &&
        point.y >= MELRectangleOriginIsCenterGetTop(frame) &&
        point.y < MELRectangleOriginIsCenterGetBottom(frame);
}

MELBoolean MELHitboxCollidesWithRectangle(MELHitbox * _Nonnull self, MELRectangle rectangle) {
    const MELRectangle frame = self->class->getFrame(self);
    const MELBoolean x = fabsf(frame.origin.x - rectangle.origin.x) <= (frame.size.width + rectangle.size.width) / 2;
    const MELBoolean y = fabsf(frame.origin.y - rectangle.origin.y) <= (frame.size.height + rectangle.size.height) / 2;
    return x && y;
}

MELBoolean MELHitboxCollidesWithHitbox(MELHitbox * _Nonnull self, MELHitbox * _Nonnull other) {
    const MELRectangle frame = self->class->getFrame(self);
    const MELRectangle otherFrame = other->class->getFrame(other);
    const MELBoolean x = fabsf(frame.origin.x - otherFrame.origin.x) <= (frame.size.width + otherFrame.size.width) / 2;
    const MELBoolean y = fabsf(frame.origin.y - otherFrame.origin.y) <= (frame.size.height + otherFrame.size.height) / 2;
    return x && y;
}

MELRectangle MELHitboxTopHalfRectangle(MELHitbox * _Nonnull self) {
    MELRectangle frame = self->class->getFrame(self);
    return (MELRectangle) {
        .origin = (MELPoint) {
            .x = frame.origin.x,
            .y = frame.origin.y - frame.size.height / 4
        },
        .size = (MELSize) {
            .width = frame.size.width,
            .height = frame.size.height / 2
        }
    };
}

MELRectangle MELHitboxBottomQuarterRectangle(MELHitbox * _Nonnull self) {
    MELRectangle frame = self->class->getFrame(self);
    return (MELRectangle) {
        .origin = (MELPoint) {
            .x = frame.origin.x,
            .y = frame.origin.y + frame.size.height * 3 / 4
        },
        .size = (MELSize) {
            .width = frame.size.width,
            .height = frame.size.height / 4
        }
    };
}
