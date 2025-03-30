//
//  simplespritehitbox.h
//  Roll
//
//  Created by Raphaël Calabro on 03/09/2022.
//

#ifndef simplespritehitbox_h
#define simplespritehitbox_h

#include "hitbox.h"

extern const MELHitboxClass MELSimpleSpriteHitboxClass;

typedef struct melsprite MELSprite;

typedef struct {
    MELHitbox super;
    MELSprite * _Nonnull sprite;
} MELSimpleSpriteHitbox;

MELHitbox * _Nonnull MELSimpleSpriteHitboxAlloc(MELSprite * _Nonnull sprite);

#endif /* simplespritehitbox_h */
