//
//  synchronizedanimation.c
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "synchronizedanimation.h"

/**
 * Start time shared by every instance of MELSynchronizedLoopingAnimation.
 */
static unsigned int referenceDate = 0;

void MELSynchronizedLoopingAnimationReset(void) {
    referenceDate = 0;
}

static void MELSynchronizedLoopingAnimationStart(MELAnimation * _Nonnull self) {
    if (referenceDate == 0) {
        referenceDate = playdate->system->getCurrentTimeMilliseconds();
    }
}

static void MELSynchronizedLoopingAnimationUpdate(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate) {
    const MELTimeInterval timeSinceStart = (playdate->system->getCurrentTimeMilliseconds() - referenceDate) / 1000.0f;
    const MELTimeInterval framesPerSecond = MELAnimationFramesPerSecond((MELAnimation *)self);
    MELAnimationSetFrameIndex(self, (int)(timeSinceStart * framesPerSecond) % self->definition->frameCount);
}

const MELAnimationClass MELSynchronizedLoopingAnimationClass = {
    .start = &MELSynchronizedLoopingAnimationStart,
    .update = &MELSynchronizedLoopingAnimationUpdate,
    .draw = &MELAnimationDraw,
    .transitionToAnimation = &MELAnimationTransitionToAnimation
};

MELAnimation * _Nonnull MELSynchronizedLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition) {
    MELAnimation *self = playdate->system->realloc(NULL, sizeof(MELAnimation));
    *self = MELAnimationMake(&MELSynchronizedLoopingAnimationClass, definition);
    return self;
}
