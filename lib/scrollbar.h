//
//  scrollbar.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 01/04/2025.
//

#ifndef scrollbar_h
#define scrollbar_h

#include "melstd.h"

#include "rectangle.h"

LCDSprite * _Nullable MELScrollBarConstructor(float top, float bottom);
LCDSprite * _Nullable MELScrollBarConstructorWithFrame(MELRectangle frame, float top, float height);
void MELScrollBarSetHeight(LCDSprite * _Nonnull sprite, float height);

#endif /* scrollbar_h */
