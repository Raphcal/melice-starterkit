//
//  subsprite.c
//  Roll
//
//  Created by Raphaël Calabro on 06/05/2022.
//

#include "subsprite.h"

#include "spritehitbox.h"
#include "simplespritehitbox.h"
#include "scene.h"

MELListImplement(MELSubSpriteRef);
MELListImplementSaveRef(MELSubSprite);

static const MELSpriteClass MELSubSpriteClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
};

static void update(LCDSprite * _Nonnull sprite);

MELSubSprite * _Nonnull MELSubSpriteAlloc(LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation) {
    MELSubSprite *self = playdate->system->realloc(NULL, sizeof(MELSubSprite));
    MELSubSpriteInit(self, parent, definition, animation);
    return self;
}

void MELSubSpriteInit(MELSubSprite * _Nonnull self, LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation) {
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setZIndex(sprite, playdate->sprite->getZIndex(parent));

    *self = (MELSubSprite) {
        .super = {
            .class = &MELSubSpriteClass,
            .definition = *definition,
            .frame = (MELRectangle) {
                .origin = MELPointZero,
                .size = definition->size
            },
            .direction = MELDirectionRight,
        },
        .sprite = sprite,
    };
    MELSpriteSetAnimation(&self->super, animation);

    if (!MELRectangleEquals(self->super.animation->frame.hitbox, MELRectangleZero)) {
        self->super.hitbox = MELSpriteHitboxAlloc(&self->super);
    } else {
        self->super.hitbox = MELSimpleSpriteHitboxAlloc(&self->super);
    }

    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->addSprite(sprite);
}

MELSubSprite * _Nonnull MELSubSpriteAllocAndPush(LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation) {
    MELSubSprite *self = MELSubSpriteAlloc(parent, definition, animation);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("MELSubSpriteAllocAndPush(%x, %x): %d", self->sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, self->sprite);
    return self;
}

void MELSubSpriteDealloc(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimationDealloc(self->animation);
    self->animation = NULL;
    if (self->hitbox != NULL) {
        MELHitboxDeinit(self->hitbox);
        playdate->system->realloc(self->hitbox, 0);
        self->hitbox = NULL;
    }
    if (self->instance != NULL) {
        self->instance->sprite = NULL;
    }
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
}

void MELSubSpriteSaveNullable(MELSubSprite * _Nullable sprite, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteBoolean(outputStream, sprite != NULL);
    if (sprite != NULL) {
        MELSubSpriteSave(sprite, outputStream);
    }
}

void MELSubSpriteSave(MELSubSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    MELSubSprite self = *sprite;

    MELOutputStreamWriteUInt16(outputStream, self.super.definition.name);
    MELOutputStreamWriteBoolean(outputStream, self.sprite != NULL);
    if (!self.sprite) {
        return;
    }

    MELOutputStreamWriteByte(outputStream, self.super.animationName);
    MELOutputStreamWriteByte(outputStream, self.super.animationDirection);
    MELAnimationSave(self.super.animation, outputStream);
    MELOutputStreamWriteRectangle(outputStream, self.super.frame);
    MELOutputStreamWriteByte(outputStream, self.super.direction);

    MELOutputStreamWriteBoolean(outputStream, self.super.hitbox != NULL);
    if (self.super.hitbox) {
        self.super.hitbox->class->save(self.super.hitbox, outputStream);
    }

    MELOutputStreamWriteShort(outputStream, playdate->sprite->getZIndex(self.sprite));
    MELOutputStreamWriteBoolean(outputStream, playdate->sprite->isVisible(self.sprite));
}

MELSubSprite * _Nullable MELSubSpriteLoadNullable(MELInputStream * _Nonnull inputStream) {
    const MELBoolean exists = MELInputStreamReadBoolean(inputStream);
    if (exists) {
        return MELSubSpriteLoad(inputStream);
    } else {
        return NULL;
    }
}

MELSubSprite * _Nullable MELSubSpriteLoad(MELInputStream * _Nonnull inputStream) {
    MELSubSprite *self = playdate->system->realloc(NULL, sizeof(MELSubSprite));

    const SpriteName name = MELInputStreamReadUInt16(inputStream);
    MELSpriteDefinition *definition = SpriteNameGetDefinition(name);
    if (!definition->palette) {
        definition->palette = SpriteNameLoadBitmapTable(name);
    }

    const MELBoolean hasSprite = MELInputStreamReadBoolean(inputStream);
    if (!hasSprite) {
        *self = (MELSubSprite) {
            .super = {
                .class = &MELSubSpriteClass,
                .definition = *definition,
                .frame = {
                    .size = definition->size,
                }
            },
        };
        return self;
    }

    LCDSprite *sprite = playdate->sprite->newSprite();

    const AnimationName animationName = (AnimationName) MELInputStreamReadByte(inputStream);
    const MELAnimationDirection animationDirection = (MELAnimationDirection) MELInputStreamReadByte(inputStream);
    MELAnimationDefinition *animationDefinition = MELSpriteDefinitionGetAnimationDefinition(*definition, animationName, animationDirection);
    MELAnimation *animation = MELAnimationLoad(inputStream, animationDefinition);
    const MELRectangle frame = MELInputStreamReadRectangle(inputStream);
    const MELDirection direction = (MELDirection) MELInputStreamReadByte(inputStream);

    *self = (MELSubSprite) {
        .super = {
            .class = &MELSubSpriteClass,
            .definition = *definition,
            .animationName = animationName,
            .animationDirection = animationDirection,
            .animation = animation,
            .frame = frame,
            .direction = direction,
        },
        .sprite = sprite,
    };

    const MELBoolean hasHitbox = MELInputStreamReadBoolean(inputStream);
    if (hasHitbox) {
        self->super.hitbox = MELHitboxLoad(inputStream, &self->super);
    }

    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, MELInputStreamReadShort(inputStream));
    playdate->sprite->setVisible(sprite, MELInputStreamReadBoolean(inputStream));
    playdate->sprite->addSprite(sprite);

    if (self->super.animation) {
        MELAnimationDraw(self->super.animation, sprite);
    }
    return self;
}

void MELSubSpriteRefDeinit(MELSubSpriteRef * _Nonnull self) {
    playdate->sprite->setUpdateFunction((*self)->sprite, MELSubSpriteDealloc);
}

void MELSubSpriteDeallocFromParent(MELSubSprite * _Nonnull self) {
    if (self->sprite) {
        // Utilisation de MELSpriteDealloc car les sprites qui appellent cette méthode push les subsprites vers la liste sprites.
        playdate->sprite->setUpdateFunction(self->sprite, MELSpriteDealloc);
    } else {
        playdate->system->realloc(self, 0);
    }
}

void MELSubSpriteDetachFromParent(MELSprite * _Nonnull sprite) {
    MELSubSprite *self = (MELSubSprite *)sprite;
    if (self->parentRef) {
        *self->parentRef = NULL;
        self->parentRef = NULL;
    }
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimation *animation = self->animation;
    MELAnimationUpdate(animation, DELTA);

    MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), MELDirectionFlip[self->direction]);
}
