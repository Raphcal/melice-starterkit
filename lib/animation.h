//
//  animation.h
//  shmup
//
//  Created by Raphaël Calabro on 28/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef animation_h
#define animation_h

#include "melstd.h"

#include "animationdefinition.h"
#include "animationframe.h"
#include "inputstream.h"
#include "outputstream.h"

/**
 * Instance of an animation.
 */
typedef struct melanimation MELAnimation;

/**
 * Class of an animation.
 */
typedef struct {

    /**
     * Updates the animation.
     *
     * @param self Animation instance.
     * @param timeSinceLastUpdate Time elapsed in seconds between now and the last call to update.
     */
    void (* _Nonnull update)(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate);

    /**
     * Draws the current animation frame.
     *
     * @param self Animation instance.
     * @param sprite Sprite to animate.
     */
    void (* _Nonnull draw)(MELAnimation * _Nonnull self, LCDSprite * _Nonnull sprite);

    /**
     * Starts the animation.
     *
     * @param self Animation instance.
     */
    void (* _Nonnull start)(MELAnimation * _Nonnull self);

    /**
     * Returns the next animation to use.
     *
     * @param self Animation instance.
     * @param nextAnimation Animation to transition to.
     * @return The next animation.
     */
    MELAnimation (* _Nonnull transitionToAnimation)(MELAnimation * _Nonnull self, MELAnimation nextAnimation);

    /**
     * Write the animation type and its current state to the given `outputStream`.
     *
     * @param self Animation instance.
     * @param outputStream Stream to write to.
     */
    void (* _Nullable save)(MELAnimation * _Nonnull self, MELOutputStream * _Nonnull outputStream);

} MELAnimationClass;

struct melanimation {

    /**
     * Class of this animation. Defines the virtual functions.
     */
    const MELAnimationClass * _Nonnull class;

    /**
     * Definition of this animation.
     */
    MELAnimationDefinition * _Nullable definition;

    /**
     * Index of the current frame.
     */
    unsigned int frameIndex;

    /**
     * Current frame.
     */
    MELAnimationFrame frame;

    /**
     * Speed of the animation.
     */
    MELTimeInterval speed;

};

MELAnimation MELAnimationMake(const MELAnimationClass * _Nonnull class, MELAnimationDefinition * _Nullable definition);

MELAnimation * _Nonnull MELAnimationAlloc(MELAnimationDefinition * _Nullable definition);
void MELAnimationSave(MELAnimation * _Nullable self, MELOutputStream * _Nonnull outputStream);
MELAnimation * _Nullable MELAnimationLoad(MELInputStream * _Nonnull inputStream, MELAnimationDefinition * _Nullable definition);

void MELAnimationDealloc(MELAnimation * _Nullable self);

/**
 * Default update function.
 */
void MELAnimationNoopUpdate(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate);

void MELAnimationUpdate(MELAnimation * _Nonnull self, MELTimeInterval timeSinceLastUpdate);

/**
 * Default start function.
 */
void MELAnimationNoopStart(MELAnimation * _Nonnull self);

/**
 * Default draw function.
 */
void MELAnimationDraw(MELAnimation * _Nonnull self, LCDSprite * _Nonnull sprite);

/**
 * Default transitionToAnimation function.
 * Returns the given next animation as is.
 *
 * @param self Animation instance.
 * @param nextAnimation Animation to transition to.
 * @return The given animation.
 */
MELAnimation MELAnimationTransitionToAnimation(MELAnimation * _Nonnull self, MELAnimation nextAnimation);

MELTimeInterval MELAnimationFramesPerSecond(MELAnimation * _Nonnull self);

void MELAnimationSetFrameIndex(MELAnimation * _Nonnull self, int index);

MELBoolean MELAnimationIsLastFrame(const MELAnimation * restrict _Nonnull self);

#endif /* animation_h */
