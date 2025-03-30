//
//  geomap.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 04/12/2023.
//

#ifndef geomap_h
#define geomap_h

#include "melstd.h"
#include "sprite.h"
#include "list.h"
#include "keyvaluetable.h"

// NOTE: Inversion des valeurs car l'écran est en vertical pour ColdBird
#define kMELGeoScreenWidth LCD_ROWS
#define kMELGeoScreenHeight LCD_COLUMNS

#define kMELGeoMapCellWidth 40
#define kMELGeoMapCellHeight 40
#define kMELGeoMapCellsInARow (kMELGeoScreenWidth / kMELGeoMapCellWidth)
#define kMELGeoMapCellsInAColumn (kMELGeoScreenHeight / kMELGeoMapCellHeight)
#define kMELGeoMapBucketCount (kMELGeoMapCellsInARow * kMELGeoMapCellsInAColumn)

MELListDefine(MELPointer);
MELKeyValueTableDefine(MELPointer, MELBoolean);

typedef struct {
    uint32_t capacity;
    LCDSpriteRef * _Nullable memory;
} MELGeoMapBucket;

typedef struct geomap {
    uint16_t counts[kMELGeoMapBucketCount];
    MELGeoMapBucket buckets[kMELGeoMapBucketCount];
} MELGeoMap;

typedef struct geomapiterator {
    MELGeoMap * _Nonnull geoMap;
    MELIntRectangle rectangle;
    int count;
    int index;
    int cellIndex;
    MELPointerMELBooleanTable set;
    MELPointerMELBooleanTable exclusions;
} MELGeoMapIterator;

MELGeoMap * _Nonnull MELGeoMapAlloc(void);
void MELGeoMapDeinit(MELGeoMap * _Nonnull self);
void MELGeoMapClear(MELGeoMap * _Nonnull self);
void MELGeoMapPutSprite(MELGeoMap * _Nonnull self, LCDSprite * _Nonnull sprite);
LCDSpriteRefList MELGeoMapSpriteListAtPoint(MELGeoMap * _Nonnull self, MELPoint point);
MELGeoMapIterator * _Nonnull MELGeoMapSpriteIteratorInRectangle(MELGeoMap * _Nonnull self, MELRectangle rectangle);
void MELGeoMapSpritesInRectangleWithIterator(MELGeoMap * _Nonnull self, MELRectangle rectangle, MELGeoMapIterator * _Nonnull iterator, MELPointerList exclusions);
MELGeoMapIterator * _Nonnull MELGeoMapIteratorAlloc(void);
void MELGeoMapIteratorDealloc(MELGeoMapIterator * _Nonnull self);
MELBoolean MELGeoMapIteratorHasNext(MELGeoMapIterator * _Nonnull self);
LCDSprite * _Nonnull MELGeoMapIteratorNext(MELGeoMapIterator * _Nonnull self);

#endif /* geomap_h */
