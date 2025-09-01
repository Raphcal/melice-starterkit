//
//  sprite.c
//  Roll
//
//  Created by Raphaël Calabro on 28/08/2022.
//

#include "sprite.h"

#include "spritehitbox.h"
#include "simplespritehitbox.h"
#include "melmath.h"
#include "scene.h"
#include "camera.h"
#include "../src/gamescene.h"
#include "../src/classes.h"

LCDSprite * _Nonnull MELSpriteInit(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    MELPoint origin = instance->center;
    spriteAPI->moveTo(sprite, origin.x, origin.y);
    spriteAPI->setZIndex(sprite, instance->zIndex);

    *self = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .instance = instance,
        .frame = (MELRectangle) {
            .origin = origin,
            .size = definition->size
        },
        .direction = instance->direction,
    };
    MELSpriteSetAnimation(self, instance->animationName);

    if (!MELRectangleEquals(self->animation->frame.hitbox, MELRectangleZero)) {
        self->hitbox = MELSpriteHitboxAlloc(self);
    } else {
        self->hitbox = MELSimpleSpriteHitboxAlloc(self);
    }

    spriteAPI->setUserdata(sprite, self);
    spriteAPI->addSprite(sprite);

    instance->sprite = sprite;
    return sprite;
}

LCDSprite * _Nonnull MELSpriteInitWithCenter(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELPoint center) {
    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->moveTo(sprite, center.x, center.y);

    *self = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .frame = (MELRectangle) {
            .origin = center,
            .size = definition->size
        },
        .direction = MELDirectionRight,
    };
    MELSpriteSetAnimation(self, AnimationNameStand);

    if (!MELRectangleEquals(self->animation->frame.hitbox, MELRectangleZero)) {
        self->hitbox = MELSpriteHitboxAlloc(self);
    } else {
        self->hitbox = MELSimpleSpriteHitboxAlloc(self);
    }

    spriteAPI->setUserdata(sprite, self);
    spriteAPI->addSprite(sprite);
    return sprite;
}

LCDSprite * _Nonnull MELSpriteInitHiddenWithUpdate(MELSprite * _Nonnull self, void (* _Nullable update)(LCDSprite * _Nonnull)) {
    *self = (MELSprite) {
        .class = &MELSpriteClassDefault,
    };

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->setVisible(sprite, false);
    spriteAPI->addSprite(sprite);
    return sprite;
}

void MELSpriteDealloc(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("MELSpriteDealloc(%x, %x): %d", sprite, self, self->definition.name);
    int index = LCDSpriteRefListRemoveSwapEntry(&currentScene->sprites, sprite);
    if (index < 0) {
        playdate->system->logToConsole("LCDSpriteRefListRemoveSwapEntry: sprite %x not found (%d)", sprite, index);
    }
#else
    LCDSpriteRefListRemoveSwapEntry(&currentScene->sprites, sprite);
#endif
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
    if (self->userdata && self->autoReleaseUserdata) {
        playdate->system->realloc(self->userdata, 0);
        self->userdata = NULL;
        self->autoReleaseUserdata = false;
    }
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
}

void MELSpriteNoopUpdate(LCDSprite * _Nonnull sprite) {
    // Do nothing.
}

void MELSpriteNoopDraw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawrect) {
    // Draw nothing.
}

/// Déplace et gère l'animation du `LCDSprite` donné.
void MELSpriteUpdate(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimation *animation = self->animation;
    MELAnimationUpdate(animation, DELTA);

    const MELSpritePositionFixed fixed = self->fixed;
    const MELPoint origin = self->frame.origin;
    const float x = (fixed & MELSpritePositionFixedX) ? origin.x : origin.x - camera.frame.origin.x;
    const float y = (fixed & MELSpritePositionFixedY) ? origin.y : origin.y - camera.frame.origin.y;
    playdate->sprite->moveTo(sprite, x, y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), MELDirectionFlip[self->direction]);
}

/// Déplace, gère l'animation et affiche un effet de collision pour le `MELSprite` donné.
void MELSpriteDraw(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimation *animation = self->animation;
    MELAnimationUpdate(animation, DELTA);

    const MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}

void MELSpriteSetAnimationAndDirection(MELSprite * _Nonnull self, AnimationName animationName, MELAnimationDirection direction) {
    MELAnimation *currentAnimation = self->animation;
    if (animationName != self->animationName || !currentAnimation || self->animationDirection != direction) {
        self->animationName = animationName;
        self->animationDirection = direction;
        MELAnimationDealloc(currentAnimation);
        self->animation = MELSpriteDefinitionGetAnimation(self->definition, animationName, direction);
    }
}

void MELSpriteSetAnimation(MELSprite * _Nonnull self, AnimationName animationName) {
    MELSpriteSetAnimationAndDirection(self, animationName, self->direction == MELDirectionRight ? MELAnimationDirectionRight : MELAnimationDirectionLeft);
}

void MELSpriteSetLeft(MELSprite * _Nonnull self, float left) {
    MELRectangle frame = self->frame;
    frame.origin.x = left + frame.size.width / 2.0f;
    self->frame = frame;
}

void MELSpriteSetRight(MELSprite * _Nonnull self, float right) {
    MELRectangle frame = self->frame;
    frame.origin.x = right - frame.size.width / 2.0f;
    self->frame = frame;
}
void MELSpriteMoveBy(MELSprite * _Nonnull self, MELPoint translation) {
    self->frame.origin = MELPointAdd(self->frame.origin, translation);
}

void MELSpriteSetPositionFixed(MELSprite * _Nonnull self, MELSpritePositionFixed fixed) {
    self->fixed = fixed;
}

void MELSpriteUpdateDisappearing(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    if (MELAnimationIsLastFrame(self->animation)) {
        MELSpriteDealloc(sprite);
        return;
    }
    const float delta = DELTA;
    self->animation->class->update(self->animation, delta);

    MELPoint origin = self->frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);

    MELAnimationFrame frame = self->animation->frame;
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, frame.atlasIndex), MELDirectionFlip[self->direction]);
}

void MELSpriteMakeDisappear(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    if (self->definition.animations[AnimationNameDisappear * MELAnimationDirectionCount + MELAnimationDirectionRight] == NULL) {
        playdate->sprite->setUpdateFunction(sprite, self->class->destroy);
        return;
    }

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("MELSpriteMakeDisappear(%x, %x): %d", sprite, self, self->definition.name);
#endif
    LCDSpriteRefListRemoveSwapEntry(&currentScene->sprites, sprite);
    MELSpriteInstance *instance = self->instance;
    if (instance) {
        instance->destroyed = true;
    }

    self->definition.type = MELSpriteTypeDecor;
    MELSpriteSetAnimation(self, AnimationNameDisappear);
    playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdateDisappearing);
}

MELBoolean MELSpriteIsLookingToward(MELSprite * _Nonnull self, MELPoint point) {
    return MELDirectionIsSameValue(self->direction, point.x - self->frame.origin.x);
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    MELSprite self = *sprite;
#if LOG_SAVES
    playdate->system->logToConsole("Save melSprite %x, name: %d, className: %d", sprite, self.definition.name, self.class->name);
    if (self.definition.name > SpriteNameMax) {
        playdate->system->error("Invalid name!");
    }
#endif
    MELOutputStreamWriteUInt16(outputStream, self.definition.name);
    MELOutputStreamWriteByte(outputStream, self.class->name);

    if (self.class->save) {
        self.class->save(sprite, outputStream);
    }

    MELOutputStreamWriteByte(outputStream, self.definition.type);
    MELOutputStreamWriteByte(outputStream, self.animationName);
    MELOutputStreamWriteByte(outputStream, self.animationDirection);
    MELAnimationSave(self.animation, outputStream);
    MELOutputStreamWriteRectangle(outputStream, self.frame);
    MELOutputStreamWriteByte(outputStream, self.direction);

    MELOutputStreamWriteBoolean(outputStream, self.hitbox != NULL);
    if (self.hitbox) {
        self.hitbox->class->save(self.hitbox, outputStream);
    }
}

static MELSprite * _Nullable load(MELInputStream * _Nonnull inputStream, LCDSprite * _Nonnull sprite) {
    MELSprite *self;
    SpriteName name = MELInputStreamReadUInt16(inputStream);
    SpriteClassName className = MELInputStreamReadByte(inputStream);

#if LOG_SAVES
    playdate->system->logToConsole("Load sprite %x, name: %d, className: %d", sprite, name, className);
    if (name > SpriteNameMax) {
        playdate->system->error("Invalid name!");
    }
#endif

    MELSpriteDefinition *definition = SpriteNameGetDefinition(name);
    if (!definition->palette) {
        definition->palette = SpriteNameLoadBitmapTable(name);
    }

    const MELSpriteClass *class = MELSpriteClassForName(className);
    if (class && class->load) {
        self = class->load(definition, sprite, inputStream);
    } else if (definition->loader) {
        self = definition->loader(definition, sprite, inputStream);
    } else {
        self = playdate->system->realloc(NULL, sizeof(MELSprite));
        *self = (MELSprite) {
            .class = &MELSpriteClassDefault
        };
        playdate->sprite->setUpdateFunction(sprite, MELSpriteUpdate);
    }

    if (self == NULL) {
        playdate->system->error("Unable to load sprite");
        return NULL;
    }

    self->definition = *definition;
    self->definition.type = (MELSpriteType) MELInputStreamReadByte(inputStream);
    self->animationName = (AnimationName) MELInputStreamReadByte(inputStream);
    self->animationDirection = (MELAnimationDirection) MELInputStreamReadByte(inputStream);
    MELAnimationDefinition *animationDefinition = MELSpriteDefinitionGetAnimationDefinition(self->definition, self->animationName, self->animationDirection);
    self->animation = MELAnimationLoad(inputStream, animationDefinition);
    self->frame = MELInputStreamReadRectangle(inputStream);
    self->direction = (MELDirection) MELInputStreamReadByte(inputStream);

    const MELBoolean hasHitbox = MELInputStreamReadBoolean(inputStream);
    if (hasHitbox) {
        self->hitbox = MELHitboxLoad(inputStream, self);
    } else {
        self->hitbox = NULL;
    }

    return self;
}

void MELSpriteSave(LCDSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    // Indique si le sprite est enregistré ou non.
    if (self->class->save == NULL) {
        // Sprite non supporté.
        MELOutputStreamWriteBoolean(outputStream, false);
        return;
    }
    // Sprite supporté.
    MELOutputStreamWriteBoolean(outputStream, true);

    save(self, outputStream);

    MELOutputStreamWriteShort(outputStream, playdate->sprite->getZIndex(sprite));
    MELOutputStreamWriteBoolean(outputStream, playdate->sprite->isVisible(sprite));
    MELOutputStreamWriteByte(outputStream, playdate->sprite->getTag(sprite));
}

LCDSprite * _Nullable MELSpriteLoad(MELInputStream * _Nonnull inputStream) {
    const MELBoolean wasSaved = MELInputStreamReadBoolean(inputStream);
    if (!wasSaved) {
        // Sprite non supporté (exemple : élément d'interface). Sera restauré ailleurs.
        return NULL;
    }

    LCDSprite *sprite = playdate->sprite->newSprite();

    MELSprite *self = load(inputStream, sprite);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push Load(%x, %x): %d", sprite, self, self->definition.name);
#endif
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, MELInputStreamReadShort(inputStream));
    playdate->sprite->setVisible(sprite, MELInputStreamReadBoolean(inputStream));
    playdate->sprite->setTag(sprite, MELInputStreamReadByte(inputStream));
    playdate->sprite->addSprite(sprite);

    if (self->animation) {
        MELAnimationDraw(self->animation, sprite);
    }
    return sprite;
}

MELRectangle MELSpriteGetFrame(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->frame;
}

float MELSpriteGetWidth(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->frame.size.width;
}

float MELSpriteGetHeight(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->frame.size.height;
}

MELRectangle LCDSpriteGetFrame(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->frame;
}
void LCDSpriteMoveBy(LCDSprite * _Nonnull sprite, MELPoint translation) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSpriteMoveBy(self, translation);
}
void LCDSpriteSetClass(LCDSprite * _Nonnull sprite, const MELSpriteClass * _Nonnull class) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->class = class;
}

void LCDSpriteSetPositionFixed(LCDSprite * _Nonnull sprite, MELSpritePositionFixed fixed) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSpriteSetPositionFixed(self, fixed);
}

const MELSpriteClass MELSpriteClassDefault = (MELSpriteClass) {
    .name = SpriteClassNameDefault,
    .destroy = MELSpriteDealloc,
    .update = MELSpriteUpdate,
};
