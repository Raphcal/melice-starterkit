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
#include "../src/player.h"

#if MEL_ORIENTATION_VERTICAL
    #define MELSpriteMoveTo(sprite, x, y) playdate->sprite->moveTo(sprite, y, LCD_ROWS - 1 - (x))
#else
    #define MELSpriteMoveTo(sprite, x, y) playdate->sprite->moveTo(sprite, x, y);
#endif

LCDSprite * _Nonnull MELSpriteInit(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance) {
    LCDSprite *sprite = playdate->sprite->newSprite();
    MELPoint origin = instance->center;
    MELSpriteMoveTo(sprite, origin.x, origin.y);
    playdate->sprite->setZIndex(sprite, instance->zIndex);

    *self = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .instance = instance,
        .frame = (MELRectangle) {
            .origin = instance->center,
            .size = definition->size
        },
        .direction = instance->direction,
        .hitTimer = 0.0f,
    };
    MELSpriteSetAnimation(self, AnimationNameStand);

    if (!MELRectangleEquals(self->animation->frame.hitbox, MELRectangleZero)) {
        self->hitbox = MELSpriteHitboxAlloc(self);
    } else {
        self->hitbox = MELSimpleSpriteHitboxAlloc(self);
    }

    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);

    instance->sprite = sprite;
    return sprite;
}

LCDSprite * _Nonnull MELSpriteInitWithCenter(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELPoint center) {
    LCDSprite *sprite = playdate->sprite->newSprite();
    MELSpriteMoveTo(sprite, center.x, center.y);

    *self = (MELSprite) {
        .class = &MELSpriteClassDefault,
        .definition = *definition,
        .frame = (MELRectangle) {
            .origin = center,
            .size = definition->size
        },
        .direction = MELDirectionRight,
        .hitTimer = 0.0f,
    };
    MELSpriteSetAnimation(self, AnimationNameStand);

    if (!MELRectangleEquals(self->animation->frame.hitbox, MELRectangleZero)) {
        self->hitbox = MELSpriteHitboxAlloc(self);
    } else {
        self->hitbox = MELSimpleSpriteHitboxAlloc(self);
    }

    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);
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

    MELPoint origin = self->frame.origin;
    MELSpriteMoveTo(sprite, origin.x - camera.frame.origin.x, origin.y - camera.frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), MELDirectionFlip[self->direction]);
}

/// Déplace, gère l'animation et affiche un effet de collision pour le `MELSprite` donné.
void MELSpriteDraw(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELAnimation *animation = self->animation;
    MELAnimationUpdate(animation, DELTA);

    const MELPoint origin = self->frame.origin;
    MELSpriteMoveTo(sprite, origin.x, origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);

    MELSpriteChangeDrawModeWhenHit(self, sprite);
}

void MELSpriteChangeDrawModeWhenHit(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const float hitTimer = MELFloatMax(self->hitTimer - DELTA, 0.0f);
    const LCDBitmapDrawMode drawMode = (int)(hitTimer * 20.0f) % 2 ? kDrawModeFillBlack : kDrawModeCopy;
    self->hitTimer = hitTimer;
    if (drawMode != self->drawMode) {
        self->drawMode = drawMode;
        playdate->sprite->setDrawMode(sprite, drawMode);
    }
}

void MELSpriteSetAnimation(MELSprite * _Nonnull self, AnimationName animationName) {
    MELAnimation *currentAnimation = self->animation;
    if (animationName != self->animationName || !currentAnimation) {
        self->animationName = animationName;
        MELAnimationDealloc(currentAnimation);
        MELAnimation *anAnimation;
        anAnimation = MELSpriteDefinitionGetAnimation(self->definition, animationName, self->direction == MELDirectionRight ? MELAnimationDirectionRight : MELAnimationDirectionLeft);
        anAnimation->class->start(anAnimation);
        self->animation = anAnimation;
    }
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

void MELSpriteUpdateDisappearing(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    if (MELAnimationIsLastFrame(self->animation)) {
        MELSpriteDealloc(sprite);
        return;
    }
    const float delta = DELTA;
    self->animation->class->update(self->animation, delta);

    MELPoint origin = self->frame.origin;
    MELSpriteMoveTo(sprite, origin.x, origin.y);

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

    MELOutputStreamWriteInt(outputStream, self.hitPoints);
    // TODO: Stocker le score ailleurs, quelque part dans la définition ?
    MELOutputStreamWriteInt(outputStream, self.score);

    // TODO: Stocker l'instance

    MELOutputStreamWriteFloat(outputStream, self.hitTimer);
    MELOutputStreamWriteByte(outputStream, self.drawMode);
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

    self->hitPoints = MELInputStreamReadInt(inputStream);
    self->score = MELInputStreamReadInt(inputStream);

    // TODO: Sauvegarder l'instance

    self->hitTimer = MELInputStreamReadFloat(inputStream);
    self->drawMode = MELInputStreamReadByte(inputStream);

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

const MELSpriteClass MELSpriteClassDefault = (MELSpriteClass) {
    .name = SpriteClassNameDefault,
    .destroy = MELSpriteDealloc,
};
