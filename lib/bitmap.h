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
#include "list.h"

typedef LCDBitmap * _Nullable LCDBitmapRef;
MELListDefine(LCDBitmapRef);
void LCDBitmapRefListDeinitAndFreeBitmaps(LCDBitmapRefList * _Nonnull self);

void MELBitmapSaveToFile(SDFile * _Nonnull file, LCDBitmap * _Nonnull bitmap);

LCDBitmap * _Nonnull LCDBitmapLoadOrError(const char * _Nonnull path);

LCDBitmapTable * _Nonnull LCDBitmapTableLoadOrError(const char * _Nonnull path);

LCDFont * _Nonnull LCDFontLoadOrError(const char * _Nonnull path);

/**
 * @brief Applies a fade effect to an `LCDBitmap` using a Bayer matrix.
 *
 * This function modifies the given bitmap by applying a fade effect based on a Bayer matrix.
 * The intensity of the fade is controlled by the `value` parameter. The function adjusts the
 * pixels of the bitmap and its mask accordingly to create the fade effect.
 *
 * @param bitmap A pointer to the LCDBitmap structure representing the image to be faded.
 * @param value The intensity of the fade effect, ranging from 0 to 100. Higher values result in a more pronounced fade.
 */
void LCDBitmapFadeImage(LCDBitmap * _Nonnull bitmap, uint8_t value);

/**
 * @param brightness Valeur de 0 à 1, 0 étant tout noir et 1 étant complètement visible.
 */
void LCDBitmapShadeImage(LCDBitmap * _Nonnull bitmap, float brightness);
LCDBitmap * _Nonnull LCDBitmapCopyAndShadeImage(LCDBitmap * _Nonnull bitmap, float brightness);

int LCDBitmapGetWidth(LCDBitmap * _Nonnull bitmap);
int LCDBitmapGetHeight(LCDBitmap * _Nonnull bitmap);
MELIntSize LCDBitmapGetSize(LCDBitmap * _Nonnull bitmap);

#endif /* bitmap_h */
