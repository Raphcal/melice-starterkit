//
//  noanimation.h
//  shmup
//
//  Created by Raphaël Calabro on 29/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef noanimation_h
#define noanimation_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"

/**
 * Class of the empty animation.
 */
extern const MELAnimationClass MELNoAnimationClass;

/**
 * Allocate an empty animation.
 * You are responsible for freeing the returned animation.
 *
 * @return An empty animation.
 */
MELAnimation * _Nonnull MELNoAnimationAlloc(MELAnimationDefinition * _Nullable definition);

#endif /* noanimation_h */
