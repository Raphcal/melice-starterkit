//
//  simplespritehitbox.c
//  Roll
//
//  Created by Raphaël Calabro on 03/09/2022.
//

#include "simplespritehitbox.h"

#include "hitboxtype.h"
#include "sprite.h"

static MELRectangle getFrame(MELSimpleSpriteHitbox * _Nonnull self) {
    return self->sprite->frame;
}

static void save(MELHitbox * _Nonnull self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteByte(outputStream, MELHitboxTypeSimpleSpriteHitbox);
}

const MELHitboxClass MELSimpleSpriteHitboxClass = {
    .getFrame = (MELRectangle(*)(MELHitbox *)) &getFrame,
    .save = save,
};

MELHitbox * _Nonnull MELSimpleSpriteHitboxAlloc(MELSprite * _Nonnull sprite) {
    MELSimpleSpriteHitbox *self = malloc(sizeof(MELSimpleSpriteHitbox));
    *self = (MELSimpleSpriteHitbox) {
        .super = (MELHitbox) {
            .class = &MELSimpleSpriteHitboxClass
        },
        .sprite = sprite
    };
    return (MELHitbox *)self;
}
