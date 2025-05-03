//
//  layersprite.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#ifndef layersprite_h
#define layersprite_h

#include "common.h"
#include "../lib/melice.h"
#include "../gen/maps.h"

typedef struct {
    MELSprite super;
    MELLayer * _Nullable layer;
    MELPoint scrollRate;
    int32_t leftPadding;
} LayerSprite;

LCDSprite * _Nonnull LayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELBoolean isRepeat);
LCDSprite * _Nonnull LayerSpriteConstructorWithLeftPadding(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, float leftPadding);

LCDSprite * _Nonnull LayerSpriteConstructorWithInstance(MELSpriteInstance instance, MELBoolean isRepeat);

#endif /* layersprite_h */
