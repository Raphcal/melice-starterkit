//
//  animationtype.c
//  shmup
//
//  Created by Raphaël Calabro on 04/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "animationtype.h"

#include "animation.h"
#include "halfloopinganimation.h"
#include "loopinganimation.h"
#include "noanimation.h"
#include "playonceanimation.h"
#include "singleframeanimation.h"
#include "synchronizedanimation.h"

MELAnimationType MELAnimationTypeForFrameCountAndLooping(int frameCount, MELBoolean looping) {
    if (frameCount == 1) {
        return MELAnimationTypeSingleFrame;
    } else if (looping) {
        return MELAnimationTypeLooping;
    } else if (frameCount > 1) {
        return MELAnimationTypePlayOnce;
    } else {
        return MELAnimationTypeNone;
    }
}

MELAnimationType MELAnimationTypeForFrameCountAndLoopingAndLoopStart(unsigned int frameCount, MELBoolean looping, unsigned int loopStart) {
    if (frameCount == 1) {
        return MELAnimationTypeSingleFrame;
    } else if (looping && loopStart == 0) {
        return MELAnimationTypeLooping;
    } else if (looping) {
        return MELAnimationTypeHalfLooping;
    } else if (frameCount > 1) {
        return MELAnimationTypePlayOnce;
    } else {
        return MELAnimationTypeNone;
    }
}

MELAnimationType MELAnimationTypeOf(MELAnimation * _Nonnull animation) {
    const MELAnimationClass *class = animation->class;
    if (class == &MELLoopingAnimationClass) {
        return MELAnimationTypeLooping;
    } else if (class == &MELPlayOnceAnimationClass) {
        return MELAnimationTypePlayOnce;
    } else if (class == &MELSingleFrameAnimationClass) {
        return MELAnimationTypeSingleFrame;
    } else if (class == &MELHalfLoopingAnimationClass) {
        return MELAnimationTypeHalfLooping;
    } else if (class == &MELSynchronizedLoopingAnimationClass) {
        return MELAnimationTypeSynchronized;
    } else {
        return MELAnimationTypeNone;
    }
}
