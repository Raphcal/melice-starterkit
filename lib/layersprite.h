//
//  layersprite.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 31/07/2023.
//

#ifndef layersprite_h
#define layersprite_h

#include "map.h"
#include "sprite.h"

typedef struct {
    MELSprite super;
    MELLayer * _Nullable layer;
    MELPoint scrollRate;
    int32_t leftPadding;
} MELLayerSprite;

LCDSprite * _Nonnull MELLayerSpriteConstructor(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, MELBoolean isRepeat);
LCDSprite * _Nonnull MELLayerSpriteConstructorWithLeftPadding(MELLayer * _Nonnull layer, LCDBitmap * _Nonnull image, float leftPadding);

LCDSprite * _Nonnull MELLayerSpriteConstructorWithInstance(MELSpriteInstance instance, MELBoolean isRepeat);

#endif /* layersprite_h */
