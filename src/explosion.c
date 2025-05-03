//
//  explosion.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/02/2023.
//

#include "explosion.h"

#include "gamescene.h"
#include "../gen/spriteexplosion.h"

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
static void update(LCDSprite * _Nonnull sprite);

static const MELSpriteClass ExplosionClass = (MELSpriteClass) {
    // .name = SpriteClassNameExplosion,
    .destroy = MELSpriteDealloc,
    .save = save,
    .load = load,
};

LCDSprite * _Nonnull ExplosionConstructor(MELPoint origin, AnimationName animationName) {
    loadSpriteExplosionPalette();
    return ExplosionConstructorWithDefinition(origin, &spriteExplosion, animationName);
}

LCDSprite * _Nonnull ExplosionConstructorWithDefinition(MELPoint origin, MELSpriteDefinition * _Nonnull definition, AnimationName animationName) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInitWithCenter(self, definition, origin);
    self->class = &ExplosionClass;
    MELSpriteSetAnimation(self, animationName);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->setZIndex(sprite, ZINDEX_EXPLOSIONS);
    // NOTE: Les explosions ne sont pas ajoutées aux sprites de la scène courante pour éviter les problèmes à la détection des collisions.
    // Faire une map de points pour trouver les ennemis proches et simplifier les collisions.
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

const MELSpriteClass * _Nonnull ExplosionGetClass(void) {
    return &ExplosionClass;
}

static void save(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    // Vide.
}

static MELSprite * _Nullable load(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, update);
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    *self = (MELSprite) {
        .class = &ExplosionClass,
    };
    return self;
}

static void update(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELAnimation *animation = self->animation;
    if (MELAnimationIsLastFrame(animation)) {
        self->class->destroy(sprite);
    } else {
        MELAnimationUpdate(animation, DELTA);

        MELPoint origin = self->frame.origin;
        playdate->sprite->moveTo(sprite, origin.x, origin.y);
        playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), kBitmapUnflipped);
    }
}
