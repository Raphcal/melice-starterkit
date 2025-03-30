//
//  map.h
//  Roll
//
//  Created by Raphaël Calabro on 20/03/2022.
//

#ifndef constmap_h
#define constmap_h

#include "melstd.h"

#include "layer.h"
#include "spriteinstance.h"
#include "rectangle.h"

typedef float (* _Nonnull MELPaletteHitbox)(uint16_t tile, float x);

typedef struct melmap {
    MELIntSize size;
    int16_t paletteName;
    // weak
    LCDBitmapTable * _Nullable palette;
    MELIntSize tileSize;
    MELPaletteHitbox xHitbox;
    MELPaletteHitbox yHitbox;
    MELIntRectangle water;
    unsigned int layerCount;
    MELLayer * _Nullable layers;
    MELLayerRefList grounds;
    MELSpriteInstanceList instances;
    MELIntPoint padding;
} MELMap;

extern const MELMap MELMapEmpty;

MELMap * _Nullable MELMapOpen(const char * _Nonnull path);
void MELMapDeinit(MELMap * _Nonnull self);
void MELMapDealloc(MELMap * _Nonnull self);

void MELMapReadHeader(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream);
void MELMapReadLayer(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream, int layerIndex);
void MELMapReadInstances(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream);

int MELMapIndexOfLayer(MELMap self, MELLayer * _Nullable layer);
int MELMapRefIndexOfLayer(MELMap * _Nonnull self, MELLayer * _Nullable layer);

#endif /* constmap_h */
