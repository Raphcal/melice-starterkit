//
//  constmap.c
//  Roll
//
//  Created by Raphaël Calabro on 20/03/2022.
//

#include "map.h"

#include "inputstream.h"

const MELMap MELMapEmpty = {};

MELMap * _Nullable MELMapOpen(const char * _Nonnull path) {
    MELInputStream inputStream = MELInputStreamOpen(path, kFileRead);

    if (!inputStream.file) {
        MELInputStreamClose(&inputStream);
        playdate->system->error("Map not found: %s", path);
        return NULL;
    }

    MELMap *self = playdate->system->realloc(NULL, sizeof(MELMap));
    MELMap map = {
        .tileSize = MELIntSizeMake(32, 32),
        .grounds = MELLayerRefListEmpty,
        .instances = MELSpriteInstanceListEmpty
    };
    map.size = MELIntSizeMake(MELInputStreamReadInt(&inputStream), MELInputStreamReadInt(&inputStream));
    map.paletteName = MELInputStreamReadShort(&inputStream);
    map.water = MELIntRectangleMake(MELInputStreamReadInt(&inputStream), MELInputStreamReadInt(&inputStream), MELInputStreamReadInt(&inputStream), MELInputStreamReadInt(&inputStream));
    map.layerCount = MELInputStreamReadInt(&inputStream);
    map.layers = playdate->system->realloc(NULL, sizeof(MELLayer) * map.layerCount);
    for (int layerIndex = 0; layerIndex < map.layerCount; layerIndex++) {
        MELLayer layer;
        layer.parent = self;
        layer.frame = MELIntRectangleMake(MELInputStreamReadInt(&inputStream),
                                          MELInputStreamReadInt(&inputStream),
                                          MELInputStreamReadInt(&inputStream),
                                          MELInputStreamReadInt(&inputStream));
        layer.scrollRate = MELPointMake(MELInputStreamReadFloat(&inputStream),
                                        MELInputStreamReadFloat(&inputStream));
        layer.isGround = MELInputStreamReadBoolean(&inputStream);
        layer.tileCount = layer.frame.size.width * layer.frame.size.height;
        layer.tiles = playdate->system->realloc(NULL, sizeof(uint16_t) * layer.tileCount);
        MELInputStreamRead(&inputStream, layer.tiles, sizeof(uint16_t) * layer.tileCount);
        map.layers[layerIndex] = layer;

        if (layer.isGround) {
            MELLayerRefListPush(&map.grounds, map.layers + layerIndex);
        }
    }
    const int instanceCount = MELInputStreamReadInt(&inputStream);
    if (instanceCount > 0) {
        map.instances = MELSpriteInstanceListMakeWithInitialCapacity(instanceCount);
        for (int instanceIndex = 0; instanceIndex < instanceCount; instanceIndex++) {
            MELSpriteInstance instance = MELSpriteInstanceMakeWithInputStream(&inputStream);
            instance.map = self;
            MELSpriteInstanceListPush(&map.instances, instance);
        }
    }
    MELInputStreamClose(&inputStream);

    *self = map;
    return self;
}

void MELMapDeinit(MELMap * _Nonnull self) {
    const unsigned int layerCount = self->layerCount;
    for (unsigned int index = 0; index < layerCount; index++) {
        MELLayerDeinit(self->layers + index);
    }
    playdate->system->realloc(self->layers, 0);
    MELLayerRefListDeinit(&self->grounds);
    MELSpriteInstanceListDeinitWithDeinitFunction(&self->instances, MELSpriteInstanceDeinit);
    self->palette = NULL;
    *self = MELMapEmpty;
}

void MELMapDealloc(MELMap * _Nonnull self) {
    MELMapDeinit(self);
    playdate->system->realloc(self, 0);
}

void MELMapReadHeader(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream) {
    MELMap map = {
        .tileSize = MELIntSizeMake(32, 32),
        .grounds = MELLayerRefListEmpty,
        .instances = MELSpriteInstanceListEmpty
    };
    map.size = MELIntSizeMake(MELInputStreamReadInt(inputStream), MELInputStreamReadInt(inputStream));
    map.paletteName = MELInputStreamReadShort(inputStream);
    map.water = MELIntRectangleMake(MELInputStreamReadInt(inputStream), MELInputStreamReadInt(inputStream), MELInputStreamReadInt(inputStream), MELInputStreamReadInt(inputStream));
    map.layerCount = MELInputStreamReadInt(inputStream);
    map.layers = playdate->system->realloc(NULL, sizeof(MELLayer) * map.layerCount);
    *self = map;
}

void MELMapReadLayer(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream, int layerIndex) {
    MELLayer layer;
    layer.parent = self;
    layer.frame = MELIntRectangleMake(MELInputStreamReadInt(inputStream),
                                      MELInputStreamReadInt(inputStream),
                                      MELInputStreamReadInt(inputStream),
                                      MELInputStreamReadInt(inputStream));
    layer.scrollRate = MELPointMake(MELInputStreamReadFloat(inputStream),
                                    MELInputStreamReadFloat(inputStream));
    layer.isGround = MELInputStreamReadBoolean(inputStream);
    layer.tileCount = layer.frame.size.width * layer.frame.size.height;
    layer.tiles = playdate->system->realloc(NULL, sizeof(uint16_t) * layer.tileCount);
    MELInputStreamRead(inputStream, layer.tiles, sizeof(uint16_t) * layer.tileCount);

    self->layers[layerIndex] = layer;

    if (layer.isGround) {
        MELLayerRefListPush(&self->grounds, self->layers + layerIndex);
    }
}

void MELMapReadInstances(MELMap * _Nonnull self, MELInputStream * _Nonnull inputStream) {
    const int instanceCount = MELInputStreamReadInt(inputStream);
    if (instanceCount > 0) {
        MELSpriteInstanceList instances = MELSpriteInstanceListMakeWithInitialCapacity(instanceCount);
        for (int instanceIndex = 0; instanceIndex < instanceCount; instanceIndex++) {
            MELSpriteInstance instance = MELSpriteInstanceMakeWithInputStream(inputStream);
            instance.map = self;
            MELSpriteInstanceListPush(&instances, instance);
        }
        self->instances = instances;
    }
}

int MELMapIndexOfLayer(MELMap self, MELLayer * _Nullable layer) {
    const long index = layer - self.layers;
    return index >= 0 && index < self.layerCount
        ? (int) index
        : -1;
}
int MELMapRefIndexOfLayer(MELMap * _Nonnull self, MELLayer * _Nullable layer) {
    const long index = layer - self->layers;
    return index >= 0 && index < self->layerCount
        ? (int) index
        : -1;
}
