//
//  stride.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#ifndef stride_h
#define stride_h

#include "melstd.h"

#include "sprite.h"
#include "melmath.h"

LCDSprite * _Nonnull MELStrideConstructor(MELSpriteDefinition * _Nonnull definition, MELPoint from, MELPoint to, float delay, float duration);
void MELStrideSpriteFromAndTo(LCDSprite * _Nonnull sprite, MELPoint from, MELPoint to, float delay, float duration);
void MELStrideSpriteTo(LCDSprite * _Nonnull sprite, MELPoint to, float delay, float duration);
void MELStrideSpriteBy(LCDSprite * _Nonnull sprite, MELPoint translation, float delay, float duration);
void MELStrideTogetherAlignedRight(LCDSprite * _Nullable sprite, LCDSprite * _Nullable spriteToFollow);
void MELStrideSkip(LCDSprite * _Nullable sprite);
void MELStrideResume(LCDSprite * _Nonnull sprite);
void MELStrideSetEasingFunction(LCDSprite * _Nonnull sprite, MELEasingFunction easingFunction);
void MELStrideSetProgress(LCDSprite * _Nonnull sprite, float progress);
void MELStrideSetFillBlack(LCDSprite * _Nonnull sprite);
void MELStrideSetDestroyWhenStrideEnds(LCDSprite * _Nonnull sprite, MELBoolean destroyWhenStrideEnds);
void MELStrideUpdate(LCDSprite * _Nonnull sprite);
MELBoolean MELStrideIsDone(LCDSprite * _Nullable sprite);

#endif /* stride_h */
