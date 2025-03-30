//
//  animationdefinition.h
//  shmup
//
//  Created by Raphaël Calabro on 28/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef animationdefinition_h
#define animationdefinition_h

#include "melstd.h"

#include "animationframe.h"
#include "animationtype.h"
#include "inputstream.h"
#include "list.h"

/**
 * Definition of an animation.
 */
typedef struct {
    /**
     * Number of frames in this animation.
     */
	unsigned int frameCount;
    /**
     * Frames.
     */
    MELAnimationFrame * _Nullable frames;
    /**
     * Number of frames by seconds.
     */
    int frequency;
    /**
     * Type of animation.
     */
    MELAnimationType type;
    /**
     * Frame index to loop from.
     */
    unsigned int loopStart;
} MELAnimationDefinition;

MELListDefine(MELAnimationDefinition);

MELAnimationDefinition MELAnimationDefinitionMakeWithAnimationDefinition(MELAnimationDefinition other);

/**
 * Returns an animation definition with the content of the given input stream.
 */
MELAnimationDefinition MELAnimationDefinitionMakeWithInputStream(MELInputStream * _Nonnull inputStream);

/**
 * Returns the duration of the given animation definition.
 *
 * @param self Animation definition instance.
 * @return The duration in seconds of the given animation definition.
 */
MELTimeInterval MELAnimationDefinitionDuration(MELAnimationDefinition self);

/**
 * Deinitialize and free the resources used by the given animation definition.
 * The definition itself is not freed.
 *
 * @param self Animation definition instance.
 */
void MELAnimationDefinitionDeinit(MELAnimationDefinition * _Nonnull self);

#endif /* animationdefinition_h */
