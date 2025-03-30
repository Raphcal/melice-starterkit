//
//  animationtype.h
//  shmup
//
//  Created by Raphaël Calabro on 29/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef animationtype_h
#define animationtype_h

#include "melstd.h"

typedef struct melanimation MELAnimation;

/**
 * Type of animation.
 */
typedef enum {
    
    /**
     * Not animated.
     */
    MELAnimationTypeNone,
    
    /**
     * Animation always displaying the same frame.
     */
    MELAnimationTypeSingleFrame,
    
    /**
     * Play an animation once.
     */
    MELAnimationTypePlayOnce,
    
    /**
     * Play an animation and restart at the first frame when the animation ends.
     */
    MELAnimationTypeLooping,
    
    /**
     * Same as looping but every instance is playing the same frame at the same time.
     */
    MELAnimationTypeSynchronized,

    /**
     * Play an animation and restart at a given frame when the animation ends.
     */
    MELAnimationTypeHalfLooping,

} MELAnimationType;

MELAnimationType MELAnimationTypeForFrameCountAndLooping(int frameCount, MELBoolean looping);

MELAnimationType MELAnimationTypeForFrameCountAndLoopingAndLoopStart(unsigned int frameCount, MELBoolean looping, unsigned int loopStart);

MELAnimationType MELAnimationTypeOf(MELAnimation * _Nonnull animation);

#endif /* animationtype_h */
