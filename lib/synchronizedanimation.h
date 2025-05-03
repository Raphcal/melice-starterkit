//
//  synchronizedanimation.h
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef synchronizedanimation_h
#define synchronizedanimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of looping animations playing the same frame at the same time.
 */
extern const MELAnimationClass MELSynchronizedLoopingAnimationClass;

MELAnimation * _Nonnull MELSynchronizedLoopingAnimationAlloc(MELAnimationDefinition * _Nonnull definition);
void MELSynchronizedLoopingAnimationReset(void);

#endif /* synchronizedanimation_h */
