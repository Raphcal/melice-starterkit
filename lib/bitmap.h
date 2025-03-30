//
//  bitmap.h
//  Roll
//
//  Created by Raphaël Calabro on 17/10/2022.
//

#ifndef bitmap_h
#define bitmap_h

#include "melstd.h"
#include "outputstream.h"

void MELBitmapSaveToFile(SDFile * _Nonnull file, LCDBitmap * _Nonnull bitmap);

LCDBitmap * _Nonnull LCDBitmapLoadOrError(const char * _Nonnull path);

LCDBitmapTable * _Nonnull LCDBitmapTableLoadOrError(const char * _Nonnull path);

LCDFont * _Nonnull LCDFontLoadOrError(const char * _Nonnull path);

void LCDBitmapFadeImage(LCDBitmap * _Nonnull bitmap, uint8_t value);

int LCDBitmapGetWidth(LCDBitmap * _Nonnull bitmap);

#endif /* bitmap_h */
