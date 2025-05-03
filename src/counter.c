//
//  counter.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/10/2023.
//

#include "counter.h"

#include "gamescene.h"

typedef struct {
    MELSprite super;
    LCDFont * _Nonnull font;
    CounterAlignment alignment;
    MELPoint anchor;
    char * _Nullable buffer;
    int bufferCapacity;
    int value;
} Counter;

static void dealloc(LCDSprite * _Nonnull sprite);
static void drawAndMove(Counter * _Nonnull self, LCDSprite * _Nonnull sprite, const int value);

static const MELSpriteClass CounterClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nonnull CounterConstructor(const MELPoint origin, const CounterAlignment alignment, LCDFont * _Nonnull font, const int value) {
    Counter *self = playdate->system->realloc(NULL, sizeof(Counter));
    *self = (Counter) {
        .super = {
            .class = &CounterClass,
        },
        .font = font,
        .alignment = alignment,
        .anchor = origin,
        .buffer = NULL,
        .bufferCapacity = 0,
        .value = value,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->addSprite(sprite);

    drawAndMove(self, sprite, value);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push CounterConstructor(%x, %x): %d", sprite, self, self->super.definition.name);
#endif
    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

void CounterSetValue(LCDSprite * _Nullable sprite, const int value) {
    if (!sprite) {
        return;
    }
    Counter *self = playdate->sprite->getUserdata(sprite);
    if (value != self->value) {
        self->value = value;

        LCDBitmap *oldImage = playdate->sprite->getImage(sprite);
        playdate->graphics->freeBitmap(oldImage);

        drawAndMove(self, sprite, value);
    }
}

void CountUpdateAnchorWithCurrentOrigin(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->super.frame;
    switch (self->alignment) {
        case kCounterAlignmentLeft:
            frame.origin.x -= frame.size.width / 2.0f;
            break;

        case kCounterAlignmentRight:
            frame.origin.x += frame.size.width / 2.0f;
            break;

        case kCounterAlignmentCenter:
            // Pas de modification.
        default:
            break;
    }
    self->anchor = frame.origin;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    Counter *self = playdate->sprite->getUserdata(sprite);
    if (self->buffer) {
        playdate->system->realloc(self->buffer, 0);
        self->buffer = NULL;
    }

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);

    MELSpriteDealloc(sprite);
}

static void drawAndMove(Counter * _Nonnull self, LCDSprite * _Nonnull sprite, const int value) {
    self->buffer = MELUInt32ToStringWithBuffer(value, self->buffer, &self->bufferCapacity);
    LCDFont *font = self->font;
    playdate->graphics->setFont(font);
    const int width = playdate->graphics->getTextWidth(font, self->buffer, self->bufferCapacity, kASCIIEncoding, 0);
    const int height = playdate->graphics->getFontHeight(font);

    MELRectangle frame;
    frame.size = (MELSize) {
        .width = width,
        .height = height
    };

    // Redessine l'image.
    LCDBitmap *image = playdate->graphics->newBitmap(width, height, kColorClear);
    playdate->graphics->pushContext(image);
    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->drawText(self->buffer, self->bufferCapacity, kASCIIEncoding, 0, 0);
    playdate->graphics->popContext();

    // Défini et déplace le sprite.
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, width, height);
    playdate->sprite->markDirty(sprite);

    frame.origin = self->anchor;
    switch (self->alignment) {
        case kCounterAlignmentLeft:
            frame.origin.x += width / 2.0f;
            break;

        case kCounterAlignmentRight:
            frame.origin.x -= width / 2.0f;
            break;

        case kCounterAlignmentCenter:
            // Pas de modification.
        default:
            break;
    }
    self->super.frame = frame;
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}
