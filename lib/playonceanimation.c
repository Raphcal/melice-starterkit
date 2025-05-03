//
//  playonceanimation.c
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "playonceanimation.h"

#include "../src/common.h"

void MELPlayOnceAnimationSkip(MELAnimation * _Nullable animation) {
    if (animation == NULL || animation->class != &MELPlayOnceAnimationClass) {
        playdate->system->logToConsole("Unable to skip given animation because it is either null or not an instance of MELPlayOneAnimation");
        return;
    }
    MELPlayOnceAnimation *self = (MELPlayOnceAnimation *)animation;
    self->startDate = 0;
}

static void MELPlayOnceAnimationStart(MELPlayOnceAnimation * _Nonnull self) {
    MELAnimationSetFrameIndex(&self->super, 0);
    self->startDate = playdate->system->getCurrentTimeMilliseconds();
}

static void MELPlayOnceAnimationUpdate(MELPlayOnceAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate) {
    const unsigned int timeSinceStart = playdate->system->getCurrentTimeMilliseconds() - self->startDate;
    const MELTimeInterval framesPerSecond = MELAnimationFramesPerSecond((MELAnimation *)self);
    const int frame = (int)(timeSinceStart / 1000.0f * framesPerSecond);
    
    const int frameCount = self->super.definition->frameCount;
    if (frame < frameCount) {
        MELAnimationSetFrameIndex(&self->super, frame);
    } else {
        MELAnimationSetFrameIndex(&self->super, frameCount - 1);
        
        MELPlayOnceAnimationOnEnd onEnd = self->onEnd;
        if (onEnd != NULL) {
            self->onEnd = NULL;
            onEnd();
        }
    }
}

static void save(MELAnimation * _Nonnull animation, MELOutputStream * _Nonnull outputStream) {
    MELPlayOnceAnimation *self = (MELPlayOnceAnimation *)animation;
#if ASSERT_PLAY_ONCE_ON_END_IS_NULL
    if (self->onEnd) {
        playdate->system->error("MELPlayOnceAnimation#onEnd is not supported.");
    }
#endif
    MELOutputStreamWriteUInt32(outputStream, self->startDate);
}

const MELAnimationClass MELPlayOnceAnimationClass = {
    .start = (void (*)(MELAnimation *)) &MELPlayOnceAnimationStart,
    .update = (void (*)(MELAnimation *, MELTimeInterval)) &MELPlayOnceAnimationUpdate,
    .draw = &MELAnimationDraw,
    .transitionToAnimation = &MELAnimationTransitionToAnimation,
    .save = save,
};

MELAnimation * _Nonnull MELPlayOnceAnimationAlloc(MELAnimationDefinition * _Nonnull definition, MELPlayOnceAnimationOnEnd onEnd) {
    MELPlayOnceAnimation *self = playdate->system->realloc(NULL, sizeof(MELPlayOnceAnimation));
    *self = (MELPlayOnceAnimation) {
        .super = MELAnimationMake(&MELPlayOnceAnimationClass, definition),
        .onEnd = onEnd,
        .startDate = 0
    };
    return (MELAnimation *)self;
}

MELAnimation * _Nonnull MELPlayOnceAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition) {
    MELPlayOnceAnimation *self = playdate->system->realloc(NULL, sizeof(MELPlayOnceAnimation));
    *self = (MELPlayOnceAnimation) {
        .super = MELAnimationMake(&MELPlayOnceAnimationClass, definition),
        .startDate = MELInputStreamReadUInt32(inputStream)
    };
    return (MELAnimation *)self;
}
