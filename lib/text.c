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

LCDSprite * _Nonnull MELTextConstructor(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    return MELImageConstructor(origin, drawText(font, drawMode, 0, text, length));
}
LCDSprite * _Nonnull MELTextConstructorWithAlignment(MELPoint origin, LCDFont * _Nonnull font, const char * _Nonnull text, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment) {
    LCDSprite *sprite = MELImageConstructor(MELPointZero, drawText(font, kDrawModeCopy, 0, text, 999));
    MELAlignmentSet(sprite, horizontalAlignment, verticalAlignment, origin);
    return sprite;
}

LCDSprite * _Nullable MELTextConstructorArgInt(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, MELHorizontalAlignment alignment, const char * _Nonnull format, int arg) {
    LCDSprite *sprite = NULL;
    char *text = NULL;
    playdate->system->formatString(&text, format, arg);
    if (text) {
        const unsigned int length = (unsigned int) strlen(text);
        sprite = MELImageConstructor(origin, drawText(font, drawMode, 0, text, length));
        free(text);
        MELAlignmentSet(sprite, alignment, MELVerticalAlignmentMiddle, origin);
    }
    return sprite;
}


LCDSprite * _Nullable MELTextConstructorArgFloat(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, MELHorizontalAlignment alignment, const char * _Nonnull format, float arg) {
    LCDSprite *sprite = NULL;
    char *text = NULL;
    playdate->system->formatString(&text, format, (double)arg);
    if (text) {
        const unsigned int length = (unsigned int) strlen(text);
        sprite = MELImageConstructor(origin, drawText(font, drawMode, 0, text, length));
        free(text);
        MELAlignmentSet(sprite, alignment, MELVerticalAlignmentMiddle, origin);
    }
    return sprite;
}

LCDSprite * _Nonnull MELTextConstructorDontPush(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    return MELImageConstructorWithSelfDontPush(self, origin, drawText(font, drawMode, 0, text, length));
}

void MELTextSet(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    MELTextSetWithLeading(sprite, font, drawMode, 0, text, length);
}

void MELTextSetWithLeading(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length) {
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
    playdate->sprite->moveTo(sprite, MOVETO_POINT(frame.origin));
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

#if MELSCREEN_ORIENTATION_VERTICAL
    LCDBitmap *target = playdate->graphics->newBitmap(height, width, kColorClear);
    gfx->pushContext(target);
    gfx->setDrawMode(drawMode);
    gfx->drawRotatedBitmap(bitmap, height / 2, width / 2, 270, 0.5f, 0.5f, 1, 1);
    gfx->popContext();
    gfx->freeBitmap(bitmap);
    bitmap = target;
#endif

    return bitmap;
}
