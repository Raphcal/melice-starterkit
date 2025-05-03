//
//  text.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 13/07/2023.
//

#ifndef text_h
#define text_h

#include "common.h"
#include "../lib/melice.h"

LCDSprite * _Nonnull TextConstructor(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);
LCDSprite * _Nonnull TextConstructorDontPush(MELPoint origin, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);

/**
 * Modifie le texte et aligne à gauche.
 */
void TextSet(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length);
void TextSetWithLeading(LCDSprite * _Nullable sprite, LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, int leading, const char * _Nonnull text, int length);

#endif /* text_h */
