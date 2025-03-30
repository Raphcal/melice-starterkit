//
//  loopinganimation.c
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "loopinganimation.h"

static void start(MELLoopingAnimation * _Nonnull self) {
    MELAnimationSetFrameIndex(&self->super, 0);
    self->time = 0;
}

static void update(MELLoopingAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate) {
    MELTimeInterval time = self->time + timeSinceLastUpdate;
    
    const MELTimeInterval framesPerSecond = MELAnimationFramesPerSecond((MELAnimation *)self);
    const int elapsedFrames = (int)(time * framesPerSecond);
    if (elapsedFrames > 0) {
        MELAnimationSetFrameIndex(&self->super, (self->super.frameIndex + elapsedFrames) % self->super.definition->frameCount);
        time -= elapsedFrames / framesPerSecond;
    }
    self->time = time;
}

static void save(MELAnimation * _Nonnull animation, MELOutputStream * _Nonnull outputStream) {
    MELLoopingAnimation *self = (MELLoopingAnimation *)animation;
    MELOutputStreamWriteFloat(outputStream, self->time);
}

const MELAnimationClass MELLoopingAnimationClass = {
    .start = (void (*)(MELAnimation *)) &start,
    .update = (void (*)(MELAnimation *, MELTimeInterval)) &update,
    .draw = &MELAnimationDraw,
    .transitionToAnimation = &MELAnimationTransitionToAnimation,
    .save = save
};

MELAnimation * _Nonnull MELLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition) {
    MELLoopingAnimation *self = playdate->system->realloc(NULL, sizeof(MELLoopingAnimation));
    *self = (MELLoopingAnimation) {
        MELAnimationMake(&MELLoopingAnimationClass, definition),
        0
    };
    return &self->super;
}

MELAnimation * _Nonnull MELLoopingAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition) {
    MELLoopingAnimation *self = playdate->system->realloc(NULL, sizeof(MELLoopingAnimation));
    *self = (MELLoopingAnimation) {
        .super = MELAnimationMake(&MELLoopingAnimationClass, definition),
        .time = MELInputStreamReadFloat(inputStream)
    };
    return &self->super;
}
