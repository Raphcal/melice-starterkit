//
//  text.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#include "text.h"

#include "image.h"

static const PDStringEncoding kDefaultEncoding = kUTF8Encoding;

static LCDBitmap * _Nonnull drawText(LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length);

LCDSprite * _Nonnull TextConstructor(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    return ImageConstructor(origin, drawText(font, drawMode, 0, text, length));
}

LCDSprite * _Nonnull TextConstructorDontPush(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    return ImageConstructorWithSelfDontPush(self, origin, drawText(font, drawMode, 0, text, length));
}

void TextSet(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    TextSetWithLeading(sprite, font, drawMode, 0, text, length);
}

void TextSetWithLeading(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length) {
    if (!sprite) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);

    image = drawText(font, drawMode, leading, text, length);
    int newWidth, newHeight;
    playdate->graphics->getBitmapData(image, &newWidth, &newHeight, NULL, NULL, NULL);
    MELRectangle frame = self->frame;
    frame.origin.x += (newWidth - frame.size.width) / 2;
    frame.origin.y += (newHeight - frame.size.height) / 2;
    frame.size.width = newWidth;
    frame.size.height = newHeight;
    self->frame = frame;

    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, newWidth, newHeight);
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
    playdate->sprite->markDirty(sprite);
}

static LCDBitmap * _Nonnull drawText(LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length) {
    int lines = 1;
    for (const char *value = text; *value; value++) {
        if (*value == '\n') {
            lines++;
        }
    }
    const struct playdate_graphics *gfx = playdate->graphics;
    const int width = gfx->getTextWidth(font, text, length, kDefaultEncoding, 0);
    const int height = (gfx->getFontHeight(font) + leading) * lines;

    LCDBitmap *bitmap = playdate->graphics->newBitmap(width, height, kColorClear);
    gfx->pushContext(bitmap);
    gfx->setFont(font);
    gfx->setDrawMode(drawMode);
    gfx->setTextLeading(leading);
    gfx->drawText(text, length, kDefaultEncoding, 0, 0);
    gfx->setTextLeading(0);
    gfx->popContext();

    return bitmap;
}
