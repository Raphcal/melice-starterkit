//
//  singleframeanimation.c
//  shmup
//
//  Created by Raphaël Calabro on 30/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "singleframeanimation.h"

#include "melmath.h"

static void MELSingleFrameAnimationStart(MELAnimation * _Nonnull self) {
    MELAnimationSetFrameIndex(self, 0);
}

const MELAnimationClass MELSingleFrameAnimationClass = {
    .start = &MELSingleFrameAnimationStart,
    .update = &MELAnimationNoopUpdate,
    .draw = &MELAnimationDraw,
    .transitionToAnimation = &MELAnimationTransitionToAnimation
};

MELAnimation MELSingleFrameAnimationMake(MELAnimationDefinition * _Nonnull definition) {
    return MELAnimationMake(&MELSingleFrameAnimationClass, definition);
}

MELAnimation * _Nonnull MELSingleFrameAnimationAlloc(MELAnimationDefinition * _Nonnull definition) {
    MELAnimation *self = playdate->system->realloc(NULL, sizeof(MELAnimation));
    *self = MELSingleFrameAnimationMake(definition);
    return self;
}

void MELSingleFrameAnimationReuse(MELAnimation * _Nonnull self) {
    self->class = &MELSingleFrameAnimationClass;
    MELAnimationSetFrameIndex(self, 0);
}

void MELSingleFrameAnimationSetAngleRadian(MELAnimation * _Nonnull self, float radian) {
    MELAnimationDefinition *definition = self->definition;
    const unsigned int frameCount = definition->frameCount;
    const unsigned int currentFrame = (radian / MEL_2_PI) * frameCount;
    MELAnimationSetFrameIndex(self, currentFrame % frameCount);
}

void MELSingleFrameAnimationSetAngleDegree(MELAnimation * _Nonnull self, float degree) {
    MELAnimationDefinition *definition = self->definition;
    const unsigned int frameCount = definition->frameCount;
    const unsigned int currentFrame = (degree / 360.0f) * frameCount;
    MELAnimationSetFrameIndex(self, currentFrame % frameCount);
}
