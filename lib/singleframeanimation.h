//
//  singleframeanimation.h
//  shmup
//
//  Created by Raphaël Calabro on 30/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef singleframeanimation_h
#define singleframeanimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of animations composed of a single frame.
 */
extern const MELAnimationClass MELSingleFrameAnimationClass;

MELAnimation * _Nonnull MELSingleFrameAnimationAlloc(MELAnimationDefinition * _Nonnull definition);
void MELSingleFrameAnimationReuse(MELAnimation * _Nonnull self);

void MELSingleFrameAnimationSetAngleRadian(MELAnimation * _Nonnull self, float radian);
void MELSingleFrameAnimationSetAngleDegree(MELAnimation * _Nonnull self, float degree);

#endif /* singleframeanimation_h */
