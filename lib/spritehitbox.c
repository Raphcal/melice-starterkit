//
//  spritehitbox.c
//  Roll
//
//  Created by Raphaël Calabro on 02/09/2022.
//

#include "spritehitbox.h"

#include "sprite.h"
#include "hitboxtype.h"

MELRectangle MELSpriteHitboxGetFrame(const MELSprite * _Nonnull sprite, const MELRectangle hitbox) {
    if (sprite->animation == NULL) {
        return MELRectangleZero;
    }
    const MELRectangle frame = sprite->frame;
    return (MELRectangle) {
        .origin = {
            .x = frame.origin.x + hitbox.origin.x * MELDirectionValues[sprite->direction],
            .y = frame.origin.y + hitbox.origin.y,
        },
        .size = hitbox.size
    };
}

static MELRectangle getFrame(MELSpriteHitbox * _Nonnull self) {
    MELSprite *sprite = self->sprite;
    return MELSpriteHitboxGetFrame(sprite, sprite->animation->frame.hitbox);
}

static void save(MELHitbox * _Nonnull self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteByte(outputStream, MELHitboxTypeSpriteHitbox);
}

const MELHitboxClass MELSpriteHitboxClass = {
    .getFrame = (MELRectangle(*)(MELHitbox *)) getFrame,
    .save = save,
};

MELSpriteHitbox MELSpriteHitboxMake(MELSprite * _Nonnull sprite) {
    return (MELSpriteHitbox) {
        .super = (MELHitbox) {
            .class = &MELSpriteHitboxClass
        },
        .sprite = sprite
    };
}

MELHitbox * _Nonnull MELSpriteHitboxAlloc(MELSprite * _Nonnull sprite) {
    MELSpriteHitbox *self = malloc(sizeof(MELSpriteHitbox));
    *self = MELSpriteHitboxMake(sprite);
    return (MELHitbox *)self;
}
