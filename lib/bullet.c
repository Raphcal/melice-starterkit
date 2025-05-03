//
//  bullet.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 09/02/2023.
//

#include "bullet.h"

#include "screen.h"
#include "spritehitbox.h"
#include "scene.h"

typedef struct {
    MELSprite super;
    MELPoint speed;
} Bullet;

static void update(LCDSprite * _Nonnull sprite);
static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

static const MELSpriteClass BulletClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = load,
};

LCDSprite * _Nonnull BulletConstructor(const MELShootingStyleDefinition * _Nonnull definition, MELPoint origin, MELPoint speed) {
    Bullet *self = playdate->system->realloc(NULL, sizeof(Bullet));
    MELSpriteDefinition *bulletDefinition = definition->bulletDefinition;

    *self = (Bullet) {
        .super = {
            .class = &BulletClass,
            .definition = *bulletDefinition,
            .frame = {
                .origin = origin,
                .size = bulletDefinition->size
            },
        },
        .speed = speed,
    };
    self->super.definition.type = MELSpriteTypeBullet;
    self->super.hitbox = MELSpriteHitboxAlloc(&self->super);
    MELSpriteSetAnimation(&self->super, definition->bulletAnimationName);

    // TODO: Gérer les animations prévues pour un angle ?

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, 10);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push Bullet(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);

    return sprite;
}

const MELSpriteClass * _Nonnull BulletGetClass(void) {
    return &BulletClass;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    Bullet *self = (Bullet *) sprite;
    MELOutputStreamWritePoint(outputStream, self->speed);
}

static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, update);

    Bullet *self = playdate->system->realloc(NULL, sizeof(Bullet));
    *self = (Bullet) {
        .super = {
            .class = &BulletClass,
        },
        .speed = MELInputStreamReadPoint(inputStream),
    };
    return &self->super;
}

static void update(LCDSprite * _Nonnull sprite) {
    Bullet *self = playdate->sprite->getUserdata(sprite);
    const float delta = DELTA;
    const MELPoint speed = self->speed;
    MELRectangle frame = self->super.frame;
    frame.origin = (MELPoint) {
        .x = frame.origin.x + speed.x * delta,
        .y = frame.origin.y + speed.y * delta,
    };
    self->super.frame = frame;

    if (!MELRectangleIntersectsWithRectangle(frame, MELScreen)) {
        MELSpriteDealloc(sprite);
        return;
    }

    MELAnimation *animation = self->super.animation;
    MELAnimationUpdate(animation, delta);

    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->super.definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
}

