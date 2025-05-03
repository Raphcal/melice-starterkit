//
//  explosion.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/02/2023.
//

#ifndef explosion_h
#define explosion_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull ExplosionConstructor(MELPoint origin, AnimationName animationName);
LCDSprite * _Nonnull ExplosionConstructorWithDefinition(MELPoint origin, MELSpriteDefinition * _Nonnull definition, AnimationName animationName);
const MELSpriteClass * _Nonnull ExplosionGetClass(void);

#endif /* explosion_h */
