//
//  playonceanimation.h
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef playonceanimation_h
#define playonceanimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of playing once animations.
 */
extern const MELAnimationClass MELPlayOnceAnimationClass;

typedef void (* _Nullable MELPlayOnceAnimationOnEnd)(void);

typedef struct {
    MELAnimation super;
    MELPlayOnceAnimationOnEnd onEnd;
    unsigned int startDate;
} MELPlayOnceAnimation;

MELAnimation * _Nonnull MELPlayOnceAnimationAlloc(MELAnimationDefinition * _Nonnull definition, MELPlayOnceAnimationOnEnd onEnd);
MELAnimation * _Nonnull MELPlayOnceAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition);

#endif /* playonceanimation_h */
