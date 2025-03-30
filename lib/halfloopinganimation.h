//
//  halfloopinganimation.h
//  Roll
//
//  Created by Raphaël Calabro on 09/08/2022.
//

#ifndef halfloopinganimation_h
#define halfloopinganimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of half looping animations.
 */
extern const MELAnimationClass MELHalfLoopingAnimationClass;

/**
 * Animation that will loop from a given frame.
 */
typedef struct {
    MELAnimation super;
    MELTimeInterval time;
} MELHalfLoopingAnimation;

MELAnimation * _Nonnull MELHalfLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition);
MELAnimation * _Nonnull MELHalfLoopingAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nonnull definition);

#endif /* halfloopinganimation_h */
