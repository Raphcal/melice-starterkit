//
//  spritehitbox.h
//  Roll
//
//  Created by Raphaël Calabro on 02/09/2022.
//

#ifndef spritehitbox_h
#define spritehitbox_h

#include "hitbox.h"

extern const MELHitboxClass MELSpriteHitboxClass;

typedef struct melsprite MELSprite;

typedef struct {
    MELHitbox super;
    MELSprite * _Nonnull sprite;
} MELSpriteHitbox;

MELSpriteHitbox MELSpriteHitboxMake(MELSprite * _Nonnull sprite);

MELHitbox * _Nonnull MELSpriteHitboxAlloc(MELSprite * _Nonnull sprite);

MELRectangle MELSpriteHitboxGetFrame(const MELSprite * _Nonnull sprite, const MELRectangle hitbox);

#endif /* spritehitbox_h */
