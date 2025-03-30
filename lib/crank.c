//
//  crank.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/07/2023.
//

#include "crank.h"

#include "pd_api.h"
#include "bitmap.h"

float MELCrankGetAcceleratedChange(void) {
    const float change = playdate->system->getCrankChange();
    return change * (1.0f / (0.2f + powf(1.04f, -fabsf(change) + 20.0f)));
}

#pragma mark - Crank Indicator

static const int crankIndicatorY = 210;
static const int bubbleWidth = 88;
static const int bubbleHeight = 52;
static const int bubbleX = LCD_COLUMNS - bubbleWidth;
static const int bubbleY = crankIndicatorY - bubbleHeight / 2;
static const int textOffset = 76;
static const int textWidth = 56;
static const int textHeight = 32;
static const int textFrameCount = 14;
static const int crankFrameCount = 12;
static const int crankFrameWidth = 52;
static const int crankFrameHeight = 38;
static const int frameCount = crankFrameCount * 3 + textFrameCount;

typedef struct melcrankindicator {
    MELBoolean clockwise;
    uint8_t currentFrame;
    unsigned int lastTime;
    LCDBitmap * _Nullable bubbleImage;
    LCDBitmapTable * _Nullable crankFrames;
    LCDBitmap * _Nullable textFrameImage;
} MELCrankIndicator;

MELCrankIndicator * _Nonnull MELCrankIndicatorConstructor(MELBoolean clockwise) {
    MELCrankIndicator *self = playdate->system->realloc(NULL, sizeof(MELCrankIndicator));
    *self = (MELCrankIndicator) {
        .clockwise = clockwise,
        .currentFrame = 0,
        .lastTime = 0,
        .bubbleImage = LCDBitmapLoadOrError("CoreLibs/assets/crank/crank-notice-bubble-1x"),
        .crankFrames = LCDBitmapTableLoadOrError("CoreLibs/assets/crank/crank-frames-1x"),
        .textFrameImage = LCDBitmapLoadOrError("CoreLibs/assets/crank/crank-notice-text-1x"),
        
    };
    return self;
}

void MELCrankIndicatorDealloc(MELCrankIndicator * _Nonnull self) {
    if (self->bubbleImage) {
        playdate->graphics->freeBitmap(self->bubbleImage);
        self->bubbleImage = NULL;
    }
    if (self->crankFrames) {
        playdate->graphics->freeBitmapTable(self->crankFrames);
        self->crankFrames = NULL;
    }
    if (self->textFrameImage) {
        playdate->graphics->freeBitmap(self->textFrameImage);
        self->textFrameImage = NULL;
    }
    playdate->system->realloc(self, 0);
}

void MELCrankIndicatorDraw(MELCrankIndicator * _Nonnull self, unsigned int xOffset, unsigned int yOffset) {
    const unsigned int currentTime = playdate->system->getCurrentTimeMilliseconds();
    int lastTime = self->lastTime;
    int delta = currentTime - lastTime;
    int currentFrame = self->currentFrame;

    // reset to start frame if :draw() hasn't been called in more than a second
    if (delta > 1000) {
        currentFrame = 0;
        delta = 0;
        lastTime = currentTime;
    }

    while (delta >= 49) {
        lastTime += 50;
        delta -= 50;
        currentFrame += 1;
        if (currentFrame > frameCount) {
            currentFrame = 0;
        }
    }

    playdate->graphics->setDrawMode(kDrawModeCopy);
    playdate->graphics->drawBitmap(self->bubbleImage, bubbleX + xOffset, bubbleY + yOffset, kBitmapUnflipped);

    if (currentFrame >= textFrameCount) {
        // draw crank frames
        LCDBitmap *frame = NULL;
        if (self->clockwise) {
            frame = playdate->graphics->getTableBitmap(self->crankFrames, (currentFrame - textFrameCount) % crankFrameCount);
        } else {
            frame = playdate->graphics->getTableBitmap(self->crankFrames, crankFrameCount - (currentFrame - textFrameCount) % crankFrameCount);
        }
        playdate->graphics->drawBitmap(frame, bubbleX + xOffset + (textOffset - crankFrameWidth) / 2, bubbleY + yOffset + (bubbleHeight - crankFrameHeight) / 2, kBitmapUnflipped);
    } else {
        // draw text
        playdate->graphics->drawBitmap(self->textFrameImage, bubbleX + xOffset + (textOffset - textWidth) / 2, bubbleY + yOffset + (bubbleHeight - textHeight) / 2, kBitmapUnflipped);
    }
    self->currentFrame = currentFrame;
    self->lastTime = lastTime;
}
