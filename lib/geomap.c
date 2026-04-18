#include "geomap.h"

#include "melmath.h"

MELListImplement(MELPointer);
MELKeyValueTableImplement(MELPointer, MELBoolean);

static int bucketIndexForPoint(MELPoint point);
static void grow(MELGeoMapBucket * _Nonnull self, unsigned int size);
static void ensureCapacity(MELGeoMapBucket * _Nonnull self, unsigned int required);
static void push(MELGeoMap * _Nonnull self, int bucketIndex, LCDSpriteRef element);

static void findNext(MELGeoMapIterator * _Nonnull self);

MELGeoMap * _Nonnull MELGeoMapAlloc(void) {
    MELGeoMap *self = playdate->system->realloc(NULL, sizeof(MELGeoMap));
    *self = (MELGeoMap) {};
    return self;
}

void MELGeoMapClear(MELGeoMap * _Nonnull self) {
    memset(self->counts, 0, sizeof(uint16_t) * kMELGeoMapBucketCount);
}

void MELGeoMapPutSprite(MELGeoMap * _Nonnull self, LCDSprite * _Nonnull sprite) {
    MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = melSprite->frame;
    MELIntPoint topLeft = (MELIntPoint) {
        .x = MELIntMax((frame.origin.x - frame.size.width / 2) / kMELGeoMapCellWidth, 0),
        .y = MELIntMax((frame.origin.y - frame.size.height / 2) / kMELGeoMapCellHeight, 0),
    };
    MELIntPoint bottomRight = (MELIntPoint) {
        .x = MELIntMin((frame.origin.x + frame.size.width / 2) / kMELGeoMapCellWidth, kMELGeoMapCellsInARow - 1),
        .y = MELIntMin((frame.origin.y + frame.size.height / 2) / kMELGeoMapCellHeight, kMELGeoMapCellsInAColumn - 1),
    };

    for (int y = topLeft.y; y <= bottomRight.y; y++) {
        for (int x = topLeft.x; x <= bottomRight.x; x++) {
            push(self, y * kMELGeoMapCellsInARow + x, sprite);
        }
    }
}

LCDSpriteRefList MELGeoMapSpriteListAtPoint(MELGeoMap * _Nonnull self, MELPoint point) {
    const int bucketIndex = bucketIndexForPoint(point);
    MELGeoMapBucket bucket = self->buckets[bucketIndex];
    return (LCDSpriteRefList) {
        .count = self->counts[bucketIndex],
        .capacity = bucket.capacity,
        .memory = bucket.memory,
    };
}

void MELGeoMapSpritesInRectangleWithIterator(MELGeoMap * _Nonnull self, MELRectangle rectangle, MELGeoMapIterator * _Nonnull iterator, MELPointerList exclusions) {
    MELIntPoint topLeft = (MELIntPoint) {
        .x = MELIntMax((rectangle.origin.x - rectangle.size.width / 2) / kMELGeoMapCellWidth, 0),
        .y = MELIntMax((rectangle.origin.y - rectangle.size.height / 2) / kMELGeoMapCellHeight, 0),
    };
    MELIntPoint bottomRight = (MELIntPoint) {
        .x = MELIntMin((rectangle.origin.x + rectangle.size.width / 2) / kMELGeoMapCellWidth, kMELGeoMapCellsInARow - 1),
        .y = MELIntMin((rectangle.origin.y + rectangle.size.height / 2) / kMELGeoMapCellHeight, kMELGeoMapCellsInAColumn - 1),
    };
    const int width = bottomRight.x - topLeft.x;
    const int height = bottomRight.y - topLeft.y;

    MELPointerMELBooleanTable set = iterator->set;
    MELPointerMELBooleanTableClear(&set);

    for (unsigned int index = 0; index < exclusions.count; index++) {
        MELPointerMELBooleanTablePut(&set, exclusions.memory[index], true);
    }

    *iterator = (MELGeoMapIterator) {
        .geoMap = self,
        .rectangle = (MELIntRectangle) {
            .origin = topLeft,
            .size = {
                .width = width,
                .height = height
            }
        },
        .count = width * height,
        .index = 0,
        .cellIndex = 0,
        .set = set,
    };
    findNext(iterator);
}

MELGeoMapIterator * _Nonnull MELGeoMapSpriteIteratorInRectangle(MELGeoMap * _Nonnull self, MELRectangle rectangle) {
    MELGeoMapIterator *iterator = MELGeoMapIteratorAlloc();
    MELGeoMapSpritesInRectangleWithIterator(self, rectangle, iterator, MELPointerListEmpty);
    return iterator;
}

MELGeoMapIterator * _Nonnull MELGeoMapIteratorAlloc(void) {
    MELGeoMapIterator *iterator = playdate->system->realloc(NULL, sizeof(MELGeoMapIterator));
    iterator->set = MELPointerMELBooleanTableEmpty;
    return iterator;
}

void MELGeoMapIteratorDealloc(MELGeoMapIterator * _Nonnull self) {
    MELPointerMELBooleanTableDeinit(&self->set);
    playdate->system->realloc(self, 0);
}

MELBoolean MELGeoMapIteratorHasNext(MELGeoMapIterator * _Nonnull self) {
    return self->index < self->count;
}

LCDSprite * _Nonnull MELGeoMapIteratorNext(MELGeoMapIterator * _Nonnull self) {
    const int index = self->index;
    const MELIntRectangle rectangle = self->rectangle;
    const int x = index % rectangle.size.width + rectangle.origin.x;
    const int y = index / rectangle.size.width + rectangle.origin.y;
    const int bucketIndex = y * kMELGeoMapCellsInARow + x;
    LCDSprite *sprite = self->geoMap->buckets[bucketIndex].memory[self->cellIndex++];
    findNext(self);
    return sprite;
}

static void findNext(MELGeoMapIterator * _Nonnull self) {
    const MELIntRectangle rectangle = self->rectangle;
    const int count = self->count;
    const MELGeoMap geoMap = *self->geoMap;
    int cellIndex = self->cellIndex;
    for (int index = self->index; index < count; index++) {
        const int x = index % rectangle.size.width + rectangle.origin.x;
        const int y = index / rectangle.size.width + rectangle.origin.y;
        const int bucketIndex = y * kMELGeoMapCellsInARow + x;
        const int cellCount = geoMap.counts[bucketIndex];
        MELGeoMapBucket bucket = geoMap.buckets[bucketIndex];
        for (int i = cellIndex; i < cellCount; i++) {
            MELBoolean wasPresent = false;
            MELPointerMELBooleanTablePutAndGetOldValue(&self->set, (MELPointer) bucket.memory[i], true, &wasPresent);
            if (!wasPresent) {
                self->index = index;
                self->cellIndex = i;
                return;
            }
        }
        cellIndex = 0;
    }
    // Pas de sprite suivant.
    self->index = self->count;
    self->cellIndex = 0;
}

static int bucketIndexForPoint(MELPoint point) {
    return kMELGeoMapCellsInARow * point.y / kMELGeoMapCellHeight + point.x / kMELGeoMapCellWidth;
}

static void grow(MELGeoMapBucket * _Nonnull self, unsigned int size) {
    self->memory = playdate->system->realloc(self->memory, size * sizeof(LCDSpriteRef));
    self->capacity = size;
}

static void ensureCapacity(MELGeoMapBucket * _Nonnull self, unsigned int required) {
    if (self->capacity < required) {
        const unsigned int newCapacity = self->capacity + (self->capacity * 2) + 1;
        grow(self, newCapacity > required ? newCapacity : required);
    }
}

static void push(MELGeoMap * _Nonnull self, int bucketIndex, LCDSpriteRef element) {
    MELGeoMapBucket *bucket = self->buckets + bucketIndex;
    const int count = self->counts[bucketIndex];
    ensureCapacity(bucket, count + 1);
    bucket->memory[count] = element;
    self->counts[bucketIndex] = count + 1;
}
