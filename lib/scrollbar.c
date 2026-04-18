//
//  scrollbar.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 01/04/2025.
//

#include "scrollbar.h"

#include "sprite.h"
#include "scene.h"
#include "melmath.h"
#include "camera.h"

#define ZINDEX_SCROLLBAR 99

typedef struct {
    MELSprite super;
    float top;
    float height;
    float ratio;
    float oldY;
} MELScrollBar;

static const LCDColor kDiagonalsPattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = black, 1 = white
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    // Mask, 0 = transparent, 1 = visible
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
    0b00010001,
    0b00100010,
    0b01000100,
    0b10001000,
};

static const int kSize = 6;
static const int kPadding = 6;
static const int kScrollBarHeight = LCD_ROWS - kPadding * 2;

static void draw(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void dealloc(LCDSprite * _Nonnull sprite);

static MELSpriteClass MELScrollBarClass = (MELSpriteClass) {
    //.name = SpriteClassNameScrollBar,
    .destroy = dealloc,
    .update = update,
};

LCDSprite * _Nullable MELScrollBarConstructor(float top, float height) {
    return MELScrollBarConstructorWithFrame((MELRectangle) {
        .origin = {
            .x = LCD_COLUMNS - kPadding - kSize / 2,
            .y = LCD_ROWS / 2
        },
        .size = {
            .width = kSize,
            .height = kScrollBarHeight,
        }
    }, top, height);
}

LCDSprite * _Nullable MELScrollBarConstructorWithFrame(MELRectangle frame, float top, float height) {
    MELScrollBar *self = new(MELScrollBar);
    *self = (MELScrollBar) {
        .super = {
            .class = &MELScrollBarClass,
            .frame = frame,
            .fixed = MELSpritePositionFixedBoth,
        },
        .top = top,
        .height = height,
        .ratio = frame.size.height / height,
        .oldY = camera.frame.origin.y,
    };

    const struct playdate_sprite *spriteAPI = playdate->sprite;
    LCDSprite *sprite = spriteAPI->newSprite();
    spriteAPI->setUserdata(sprite, self);
    spriteAPI->setUpdateFunction(sprite, update);
    spriteAPI->setImage(sprite, playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorClear), kBitmapUnflipped);
    spriteAPI->setZIndex(sprite, ZINDEX_SCROLLBAR);
    spriteAPI->moveTo(sprite, frame.origin.x, frame.origin.y);
    spriteAPI->addSprite(sprite);
    draw(sprite);
    MELSceneAddSprite(sprite);
    return sprite;
}

void MELScrollBarSetHeight(LCDSprite * _Nonnull sprite, float height) {
    MELScrollBar *self = playdate->sprite->getUserdata(sprite);
    self->height = height;
    self->ratio = self->super.frame.size.height / height;
    draw(sprite);
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    MELSpriteDealloc(sprite);
}

static void draw(LCDSprite * _Nonnull sprite) {
    MELScrollBar *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *image = playdate->sprite->getImage(sprite);

    const MELSize size = self->super.frame.size;
    const float barHeight = ceilf(LCD_ROWS * self->ratio);
    const float top = MELFloatBound(0, (camera.frame.origin.y - self->top) * self->ratio, size.height - barHeight);

    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);
    graphics->setDrawMode(kDrawModeCopy);

    graphics->fillRect(0, 0, size.width, size.height, kDiagonalsPattern);
    graphics->fillRect(0, top, size.width, barHeight, kColorBlack);

    graphics->popContext();
}

static void update(LCDSprite * _Nonnull sprite) {
    MELScrollBar *self = playdate->sprite->getUserdata(sprite);
    const float y = camera.frame.origin.y;
    if (self->oldY != y) {
        self->oldY = y;
        draw(sprite);
        playdate->sprite->markDirty(sprite);
    }

    const MELPoint origin = self->super.frame.origin;
    playdate->sprite->moveTo(sprite, origin.x, origin.y);
}
