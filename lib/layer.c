//
//  constlayer.c
//  Roll
//
//  Created by Raphaël Calabro on 22/03/2022.
//

#include "layer.h"

#include "map.h"

MELListImplement(MELLayerRef);

const MELLayer MELLayerEmpty = {};

void MELLayerDeinit(MELLayer * _Nonnull self) {
    if (self->tiles) {
        playdate->system->realloc(self->tiles, 0);
        self->tiles = NULL;
    }
    *self = MELLayerEmpty;
}

uint16_t MELLayerTileAtXAndY(MELLayer * _Nonnull self, float x, float y) {
    const MELIntSize tileSize = self->parent->tileSize;
    const MELIntRectangle frame = self->frame;
    const float left = frame.origin.x * tileSize.width;
    const float top = frame.origin.y * tileSize.height;
    const float right = left + frame.size.width * tileSize.width;
    const float bottom = top + frame.size.height * tileSize.height;
#define kTileY ((int) (y / tileSize.height) - frame.origin.y)
#define kTileX ((int) (x / tileSize.width) - frame.origin.x)
    return x >= left && x < right && y >= top && y < bottom
        ? self->tiles[kTileY * frame.size.width + kTileX]
        : kEmptyTile;
}

MELIntPoint MELLayerPointInTileAtPoint(MELLayer * _Nonnull self, MELPoint point) {
    const MELIntSize tileSize = self->parent->tileSize;
    return MELIntPointMake(((int)point.x) % tileSize.width, ((int)point.y) % tileSize.height);
}

MELBoolean MELLayerCollidesWithPoint(MELLayer * _Nonnull self, uint16_t tile, MELIntPoint pointInsideTile) {
    uint8_t *mask = NULL;
    int width = 0;
    playdate->graphics->getBitmapData(playdate->graphics->getTableBitmap(self->parent->palette, tile), &width, NULL, NULL, &mask, NULL);

    const int index = pointInsideTile.x + pointInsideTile.y * width;
    const int byteIndex = index / 8;
    const int bitIndex = (1 << 7) >> (index % 8);
    return mask == NULL || (mask[byteIndex] & bitIndex);
}

GLfloat MELLayerTileTop(MELLayer * _Nonnull self, MELPoint point) {
    MELIntSize tileSize = self->parent->tileSize;
    return ((int)point.y / tileSize.height) * tileSize.height;
}
GLfloat MELLayerTileBottom(MELLayer * _Nonnull self, MELPoint point) {
    MELIntSize tileSize = self->parent->tileSize;
    return ((int)point.y / tileSize.height) * tileSize.height + tileSize.height;
}
GLfloat MELLayerTileBorder(MELLayer * _Nonnull self, MELPoint point, MELDirection direction) {
    const MELIntSize tileSize = self->parent->tileSize;
    return (GLfloat) (((int) point.x / tileSize.width) + (int) direction) * tileSize.width;
}

GLfloat MELLayerAngleAtPoint(MELLayer * _Nonnull self, MELPoint point, MELDirection direction) {
    const uint16_t tile = MELLayerTileAtXAndY(self, point.x, point.y);
    const MELIntPoint pixel = MELLayerPointInTileAtPoint(self, point);

    const GLfloat backY = self->parent->xHitbox(tile, pixel.x);
    const GLfloat frontY = self->parent->xHitbox(tile, pixel.x + MELDirectionValues[direction]);

    return atan2f(frontY - backY, MELDirectionValues[direction]);
}

GLfloat MELLayerVerticalAngleAtPoint(MELLayer * _Nonnull self, MELPoint point, MELDirection direction) {
    const uint16_t tile = MELLayerTileAtXAndY(self, point.x, point.y);
    const MELIntPoint pixel = MELLayerPointInTileAtPoint(self, point);

    const GLfloat backX = self->parent->yHitbox(tile, pixel.y);
    const GLfloat frontX = self->parent->yHitbox(tile, pixel.y + MELDirectionValues[direction]);

    return atan2f(MELDirectionValues[direction], frontX - backX);
}
