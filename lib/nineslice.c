#include "nineslice.h"

#include "melmath.h"
#include "bitmap.h"

MELNineSlice * _Nonnull MELNineSliceMake(LCDBitmap * _Nonnull image, MELIntRectangle innerRect) {
    MELNineSlice *self = playdate->system->realloc(NULL, sizeof(MELNineSlice));
    const struct playdate_graphics *gfx = playdate->graphics;

    int width, height;
    gfx->getBitmapData(image, &width, &height, NULL, NULL, NULL);

    const int leftWidth = innerRect.origin.x;
    const int topHeight = innerRect.origin.y;
    const int rightWidth = width - (innerRect.origin.x + innerRect.size.width);
    const int bottomHeight = height - (innerRect.origin.y + innerRect.size.height);

    *self = (MELNineSlice) {
        .minSize = (MELIntSize) {
            .width = leftWidth + rightWidth,
            .height = topHeight + bottomHeight
        },
        .innerRect = innerRect,
        .backgroundColor = kColorWhite,
    };

    // cache slices
    MELIntRectangle rects[kNineSliceCount] = {
        MELIntRectangleMake(0, 0, leftWidth, topHeight), // top left
        MELIntRectangleMake(innerRect.origin.x, 0, innerRect.size.width, topHeight), // top center
        MELIntRectangleMake(innerRect.origin.x + innerRect.size.width, 0, rightWidth, topHeight), // top right

        MELIntRectangleMake(0, topHeight, leftWidth, innerRect.size.height), // middle left
        MELIntRectangleMake(innerRect.origin.x, topHeight, innerRect.size.width, innerRect.size.height), // middle center
        MELIntRectangleMake(innerRect.origin.x + innerRect.size.width, topHeight, rightWidth, innerRect.size.height), // middle right

        MELIntRectangleMake(0, topHeight + innerRect.size.height, leftWidth, bottomHeight), // bottom left
        MELIntRectangleMake(innerRect.origin.x, topHeight + innerRect.size.height, innerRect.size.width, bottomHeight), // bottom center
        MELIntRectangleMake(innerRect.origin.x + innerRect.size.width, topHeight + innerRect.size.height, rightWidth, bottomHeight), // bottom right
    };

    for (unsigned int index = 0; index < kNineSliceCount; index++) {
        MELIntRectangle rect = rects[index];
        LCDBitmap *slice = gfx->newBitmap(rect.size.width, rect.size.height, kColorClear);
        gfx->pushContext(slice);
        gfx->drawBitmap(image, -rect.origin.x, -rect.origin.y, kBitmapUnflipped);
        gfx->popContext();
        self->slices[index] = slice;
    }
    return self;
}

MELNineSlice * _Nonnull MELNineSliceMakeWithPath(const char * _Nonnull imagePath, MELIntRectangle innerRect) {
    return MELNineSliceMake(LCDBitmapLoadOrError(imagePath), innerRect);
}

MELNineSlice * _Nonnull MELNineSliceMakeWithBitmapTable(LCDBitmapTable * _Nonnull table, int index, MELIntRectangle innerRect) {
    return MELNineSliceMake(playdate->graphics->getTableBitmap(table, index), innerRect);
}

void MELNineSliceDealloc(MELNineSlice * _Nonnull self) {
    void (* const freeBitmap)(LCDBitmap *) = playdate->graphics->freeBitmap;
    for (unsigned int index = 0; index < kNineSliceCount; index++) {
        freeBitmap(self->slices[index]);
    }
    if (self->cache) {
        freeBitmap(self->cache);
        self->cache = NULL;
    }
    playdate->system->realloc(self, 0);
}

MELIntSize MELNineSliceGetSize(MELNineSlice * _Nonnull self) {
    return self->cacheSize;
}

MELIntSize MELNineSliceGetMinSize(MELNineSlice * _Nonnull self) {
    return self->minSize;
}

static void prerender(MELNineSlice * _Nonnull self, MELIntSize size) {
    self->cacheSize = size;
    MELIntRectangle innerRect = self->innerRect;
    MELIntSize minSize = self->minSize;

    innerRect.size = MELIntSizeSubstract(size, minSize);

    LCDBitmap **slices = self->slices;

    const struct playdate_graphics *gfx = playdate->graphics;
    LCDBitmap *cache = gfx->newBitmap(size.width, size.height, kColorClear);
    gfx->pushContext(cache);
    gfx->drawBitmap(slices[0], 0, 0, kBitmapUnflipped);
    if (innerRect.size.width > 0) {
        gfx->tileBitmap(slices[1], innerRect.origin.x, 0, innerRect.size.width, innerRect.origin.y, kBitmapUnflipped);
    }
    gfx->drawBitmap(slices[2], innerRect.origin.x + innerRect.size.width, 0, kBitmapUnflipped);

    if (innerRect.size.height > 0) {
        gfx->tileBitmap(slices[3], 0, innerRect.origin.y, innerRect.origin.x, innerRect.size.height, kBitmapUnflipped);
        if (innerRect.size.width > 0) {
            gfx->fillRect(innerRect.origin.x, innerRect.origin.y, innerRect.size.width, innerRect.size.height, self->backgroundColor);
        }
        gfx->tileBitmap(slices[5], innerRect.origin.x + innerRect.size.width, innerRect.origin.y, size.width - (innerRect.origin.x + innerRect.size.width), innerRect.size.height, kBitmapUnflipped);
    }

    gfx->drawBitmap(slices[6], 0, innerRect.origin.y + innerRect.size.height, kBitmapUnflipped);
    if (innerRect.size.width > 0) {
        gfx->tileBitmap(slices[7], innerRect.origin.x, innerRect.origin.y + innerRect.size.height, innerRect.size.width, size.height - (innerRect.origin.y + innerRect.size.height), kBitmapUnflipped);
    }
    gfx->drawBitmap(slices[8], innerRect.origin.x + innerRect.size.width, innerRect.origin.y + innerRect.size.height, kBitmapUnflipped);

    gfx->popContext();
    if (self->cache != NULL) {
        gfx->freeBitmap(self->cache);
    }
    self->cache = cache;
}

void MELNineSliceDrawInRect(MELNineSlice * _Nonnull self, MELRectangle rectangle) {
    MELIntSize minSize = self->minSize;
    MELIntSize size = (MELIntSize) {
        .width = MELIntMax(rectangle.size.width, minSize.width),
        .height = MELIntMax(rectangle.size.height, minSize.height)
    };
    if (!MELIntSizeEquals(size, self->cacheSize)) {
        prerender(self, size);
    }
    if (self->cache) {
        playdate->graphics->drawBitmap(self->cache, rectangle.origin.x, rectangle.origin.y, kBitmapUnflipped);
    }
}
