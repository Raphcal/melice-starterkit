//
//  loopinganimation.h
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef loopinganimation_h
#define loopinganimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of looping animations.
 */
extern const MELAnimationClass MELLoopingAnimationClass;

typedef struct {
    MELAnimation super;
    MELTimeInterval time;
} MELLoopingAnimation;

MELAnimation * _Nonnull MELLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition);
MELAnimation * _Nonnull MELLoopingAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition);

#endif /* loopinganimation_h */
