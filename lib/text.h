//
//  text.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#ifndef text_h
#define text_h

#include "melstd.h"

#include "point.h"
#include "alignment.h"

LCDSprite * _Nonnull MELTextConstructor(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);
LCDSprite * _Nonnull MELTextConstructorWithAlignment(MELPoint origin, LCDFont * _Nonnull font, const char * _Nonnull text, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);
LCDSprite * _Nullable MELTextConstructorArgInt(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, MELHorizontalAlignment alignment, const char * _Nonnull format, int arg);
LCDSprite * _Nullable MELTextConstructorArgFloat(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, MELHorizontalAlignment alignment, const char * _Nonnull format, float arg);
LCDSprite * _Nonnull MELTextConstructorDontPush(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);

/**
 * Modifie le texte et aligne à gauche.
 */
void MELTextSet(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);
void MELTextSetWithLeading(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length);

#endif /* text_h */
