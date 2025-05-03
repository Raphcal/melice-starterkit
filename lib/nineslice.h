//
//  nineslice.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 11/06/2024.
//

#ifndef nineslice_h
#define nineslice_h

#include "melstd.h"

#include "rectangle.h"

#define kNineSliceCount 9

typedef struct {
    MELIntRectangle innerRect;
    LCDBitmap * _Nullable slices[kNineSliceCount];
    LCDBitmap * _Nullable cache;
    MELIntSize cacheSize;
    MELIntSize minSize;
    LCDColor backgroundColor;
} MELNineSlice;

MELNineSlice * _Nonnull MELNineSliceMake(LCDBitmap * _Nonnull image, MELIntRectangle innerRect);
MELNineSlice * _Nonnull MELNineSliceMakeWithPath(const char * _Nonnull imagePath, MELIntRectangle innerRect);
MELNineSlice * _Nonnull MELNineSliceMakeWithBitmapTable(LCDBitmapTable * _Nonnull table, int index, MELIntRectangle innerRect);
void MELNineSliceDealloc(MELNineSlice * _Nonnull self);

MELIntSize MELNineSliceGetSize(MELNineSlice * _Nonnull self);
MELIntSize MELNineSliceGetMinSize(MELNineSlice * _Nonnull self);

void MELNineSliceDrawInRect(MELNineSlice * _Nonnull self, MELRectangle rectangle);

#endif /* nineslice_h */
