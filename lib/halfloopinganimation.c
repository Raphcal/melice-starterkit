//
//  halfloopinganimation.c
//  Roll
//
//  Created by Raphaël Calabro on 09/08/2022.
//

#include "halfloopinganimation.h"

static void start(MELHalfLoopingAnimation *self) {
    self->time = 0;
}

static void update(MELHalfLoopingAnimation *self, MELTimeInterval timeSinceLastUpdate) {
    const MELTimeInterval time = self->time + timeSinceLastUpdate;

    const MELAnimationDefinition definition = *self->super.definition;

    const MELTimeInterval framesPerSecond = MELAnimationFramesPerSecond((MELAnimation *)self);
    const int elapsedFrames = (int)(time * framesPerSecond);
    unsigned int frameIndex = self->super.frameIndex + elapsedFrames;
    if (frameIndex >= definition.frameCount) {
        const int loopFrameCount = definition.frameCount - definition.loopStart;
        frameIndex = definition.loopStart + elapsedFrames % loopFrameCount;
    }
    MELAnimationSetFrameIndex(&self->super, frameIndex);
    self->time = time - elapsedFrames / framesPerSecond;
}

static void save(MELAnimation * _Nonnull animation, MELOutputStream * _Nonnull outputStream) {
    MELHalfLoopingAnimation *self = (MELHalfLoopingAnimation *)animation;
    MELOutputStreamWriteFloat(outputStream, self->time);
}

const MELAnimationClass MELHalfLoopingAnimationClass = (MELAnimationClass) {
    .start = (void (*)(MELAnimation *)) &start,
    .update = (void (*)(MELAnimation *, MELTimeInterval)) &update,
    .draw = &MELAnimationDraw,
    .transitionToAnimation = &MELAnimationTransitionToAnimation,
    .save = save
};

MELAnimation * _Nonnull MELHalfLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition) {
    MELHalfLoopingAnimation *self = playdate->system->realloc(NULL, sizeof(MELHalfLoopingAnimation));
    *self = (MELHalfLoopingAnimation) {
        .super = MELAnimationMake(&MELHalfLoopingAnimationClass, definition),
        .time = 0
    };
    return (MELAnimation *)self;
}

MELAnimation * _Nonnull MELHalfLoopingAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition) {
    MELHalfLoopingAnimation *self = playdate->system->realloc(NULL, sizeof(MELHalfLoopingAnimation));
    *self = (MELHalfLoopingAnimation) {
        .super = MELAnimationMake(&MELHalfLoopingAnimationClass, definition),
        .time = MELInputStreamReadFloat(inputStream)
    };
    return (MELAnimation *)self;
}
