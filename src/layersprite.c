//
//  layersprite.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#include "layersprite.h"

#include "image.h"
#include "../lib/camera.h"

static const int32_t kMapWidth = 32 * 60;

static void update(LCDSprite * _Nonnull sprite);
static void updateRepeat(LCDSprite * _Nonnull sprite);
static void dealloc(LCDSprite * _Nonnull sprite);

const MELSpriteClass LayerSpriteClass = (MELSpriteClass) {
    .destroy = dealloc,
};

static LCDSprite * _Nonnull constructor(LayerSprite * _Nonnull self, MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, float leftPadding) {
    const int layerIndex = (int) (layer - layer->parent->layers);
    MELIntSize tileSize = layer->parent->tileSize;
    MELIntRectangle layerFrame = layer->frame;
    MELPoint origin = (MELPoint) {
        .x = layerFrame.origin.x * tileSize.width,
        .y = layerFrame.origin.y * tileSize.height
    };

    *self = (LayerSprite) {
        .super = {
            .class = &LayerSpriteClass,
            .frame = {
                .size = {
                    .width = layerFrame.size.width * tileSize.width,
                    .height = layerFrame.size.height * tileSize.width
                },
                .origin = origin,
            },
        },
        .layer = layer,
        .scrollRate = layer->scrollRate,
        .leftPadding = leftPadding,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_BG + ZINDEX_LAYER_MULTIPLIER * layerIndex);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

LCDSprite * _Nonnull LayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELBoolean isRepeat) {
    LayerSprite *self = playdate->system->realloc(NULL, sizeof(LayerSprite));

    const int32_t mapWidth = layer->parent->size.width;
#if CHECK_LAYERSPRITE_MAP_WIDTH
    if (isRepeat && mapWidth != kMapWidth) {
        playdate->system->error("LayerSpriteConstructor: expected mapWidth of %d but was: %d", kMapWidth, mapWidth);
    }
#endif

    LCDSprite *sprite = constructor(self, layer, image, isRepeat * kMapWidth);
    playdate->sprite->setUpdateFunction(sprite, mapWidth == kMapWidth ? updateRepeat : update);
    return sprite;
}

LCDSprite * _Nonnull LayerSpriteConstructorWithLeftPadding(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, float leftPadding) {
    LayerSprite *self = playdate->system->realloc(NULL, sizeof(LayerSprite));
    LCDSprite *sprite = constructor(self, layer, image, leftPadding);
    playdate->sprite->setUpdateFunction(sprite, update);
    update(sprite);
    return sprite;
}

LCDSprite * _Nonnull LayerSpriteConstructorWithInstance(MELSpriteInstance instance, MELBoolean isRepeat) {
    LayerSprite *self = new(LayerSprite);

    MELSpriteDefinition *definition = SpriteNameGetDefinition(instance.name);
    if (!definition->palette) {
        definition->palette = SpriteNameLoadBitmapTable(instance.name);
    }
    const MELSize size = definition->size;

    *self = (LayerSprite) {
        .super = {
            .class = &LayerSpriteClass,
            .definition = *definition,
            .frame = {
                .size = size,
                .origin = {
                    .x = instance.center.x - size.width / 2,
                    .y = instance.center.y - size.height / 2,
                },
            },
        },
        .scrollRate = (MELPoint) {
            .x = 1.0f,
            .y = 1.0f,
        },
        .leftPadding = isRepeat * size.width,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(definition->palette, 0), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);
    playdate->sprite->setUpdateFunction(sprite, updateRepeat);
    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->leftPadding == 0.0f) {
        LCDBitmap *bitmap = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(bitmap);
    }
    MELSpriteDealloc(sprite);
}

/**
 * Fait un scrolling sans répétition pour une carte de n'importe quelle taille.
 *
 * @param sprite Couche d'une carte.
 */
static void update(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);

    const MELRectangle frame = self->super.frame;
    const MELPoint scrollRate = self->scrollRate;

    playdate->sprite->moveTo(sprite, frame.origin.x + self->leftPadding - camera.frame.origin.x * scrollRate.x + frame.size.width / 2.0f, frame.origin.y - camera.frame.origin.y * scrollRate.y + frame.size.height / 2.0f);
}

/**
 * Fait un scrolling infini horizontalement pour une carte faisant 1920 pixels de large.
 *
 * @param sprite Couche d'une carte.
 */
static void updateRepeat(LCDSprite * _Nonnull sprite) {
    LayerSprite *self = playdate->sprite->getUserdata(sprite);

    const MELRectangle frame = self->super.frame;
    const MELPoint scrollRate = self->scrollRate;
    const int32_t leftPadding = self->leftPadding;
    const int32_t left = (int32_t)(frame.origin.x - camera.frame.origin.x * scrollRate.x) % ((int)frame.size.width);

    playdate->sprite->moveTo(sprite, left + leftPadding + frame.size.width / 2.0f, frame.origin.y - camera.frame.origin.y * scrollRate.y + frame.size.height / 2.0f);
}
