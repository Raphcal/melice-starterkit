//
//  animation.c
//  shmup
//
//  Created by Raphaël Calabro on 28/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "animation.h"

#include "noanimation.h"
#include "singleframeanimation.h"
#include "playonceanimation.h"
#include "loopinganimation.h"
#include "halfloopinganimation.h"
#include "synchronizedanimation.h"
#include "sprite.h"

MELAnimation MELAnimationMake(const MELAnimationClass * _Nonnull class, MELAnimationDefinition * _Nullable definition) {
    return (MELAnimation) {
        .class = class,
        .definition = definition,
        .frameIndex = 0,
        .frame = {},
        .speed = 1
    };
}

void MELAnimationUpdate(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate) {
    self->class->update(self, timeSinceLastUpdate);
}

void MELAnimationNoopUpdate(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate) {
    // Noop
}

void MELAnimationNoopStart(MELAnimation * _Nonnull self) {
    // Noop
}

void MELAnimationDraw(MELAnimation * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(melSprite->definition.palette, self->frame.atlasIndex), MELDirectionFlip[melSprite->direction]);
}

MELAnimation MELAnimationTransitionToAnimation(MELAnimation * _Nonnull self, MELAnimation nextAnimation) {
    return nextAnimation;
}

MELTimeInterval MELAnimationFramesPerSecond(MELAnimation * _Nonnull self) {
    return self->speed * self->definition->frequency;
}

void MELAnimationSetFrameIndex(MELAnimation * _Nonnull self, int index) {
    self->frameIndex = index;
    self->frame = self->definition->frames[index];
}

MELAnimation * _Nonnull MELAnimationAlloc(MELAnimationDefinition * _Nullable definition) {
    switch (definition != NULL ? definition->type : MELAnimationTypeNone) {
        case MELAnimationTypeSingleFrame:
            return MELSingleFrameAnimationAlloc(definition);
        case MELAnimationTypePlayOnce:
            return MELPlayOnceAnimationAlloc(definition, NULL);
        case MELAnimationTypeLooping:
            return MELLoopingAnimationAlloc(definition);
        case MELAnimationTypeSynchronized:
            return MELSynchronizedLoopingAnimationAlloc(definition);
        default:
            return MELNoAnimationAlloc(definition);
    }
}

MELBoolean MELAnimationIsLastFrame(const MELAnimation * restrict _Nonnull self) {
    return self->frameIndex == self->definition->frameCount - 1;
}

void MELAnimationDealloc(MELAnimation * _Nullable self) {
    if (self != NULL) {
        playdate->system->realloc(self, 0);
    }
}

void MELAnimationSave(MELAnimation * _Nullable self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteBoolean(outputStream, self != NULL);
    if (self == NULL) {
        return;
    }
    const MELAnimationType type = MELAnimationTypeOf(self);
    MELOutputStreamWriteByte(outputStream, type);
    if (self->class->save) {
        self->class->save(self, outputStream);
    }
    MELOutputStreamWriteUInt16(outputStream, self->frameIndex);
    MELOutputStreamWriteUInt16(outputStream, self->frame.atlasIndex);
    MELOutputStreamWriteRectangle(outputStream, self->frame.hitbox);
    MELOutputStreamWriteFloat(outputStream, self->speed);
}

MELAnimation * _Nullable MELAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nullable definition) {
    const MELBoolean hasAnimation = MELInputStreamReadBoolean(inputStream);
    if (!hasAnimation) {
        return NULL;
    }

    MELAnimationType type = MELInputStreamReadByte(inputStream);

    MELAnimation *self;
    switch (type) {
        case MELAnimationTypeNone:
            self = MELNoAnimationAlloc(definition);
            break;
        case MELAnimationTypeSingleFrame:
            self = MELSingleFrameAnimationAlloc(definition);
            break;
        case MELAnimationTypePlayOnce:
            self = MELPlayOnceAnimationLoad(inputStream, definition);
            break;
        case MELAnimationTypeLooping:
            self = MELLoopingAnimationLoad(inputStream, definition);
            break;
        case MELAnimationTypeSynchronized:
            self = MELSynchronizedLoopingAnimationAlloc(definition);
            break;
        case MELAnimationTypeHalfLooping:
            self = MELHalfLoopingAnimationLoad(inputStream, definition);
            break;
        default:
            playdate->system->error("Unsupported animation type: %d", type);
            self = MELNoAnimationAlloc(definition);
            break;
    }

    self->frameIndex = MELInputStreamReadUInt16(inputStream);
    self->frame.atlasIndex = MELInputStreamReadUInt16(inputStream);
    self->frame.hitbox = MELInputStreamReadRectangle(inputStream);
    self->speed = MELInputStreamReadFloat(inputStream);

    return self;
}
