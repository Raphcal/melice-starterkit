//
//  spritedefinition.h
//  Roll
//
//  Created by Raphaël Calabro on 27/03/2022.
//

#ifndef constspritedefinition_h
#define constspritedefinition_h

#include "melstd.h"

#include "animation.h"
#include "animationdefinition.h"
#include "animationtype.h"
#include "animationdirection.h"
#include "spritetype.h"
#include "lcdspriteref.h"
#include "../gen/spritenames.h"

typedef struct melsprite MELSprite;
typedef struct melspritedefinition MELSpriteDefinition;
typedef struct melspriteinstance MELSpriteInstance;

typedef LCDSprite * _Nonnull (* _Nullable MELSpriteConstructor)(MELSpriteDefinition * _Nonnull self, MELSpriteInstance * _Nonnull instance);
typedef MELSprite * _Nullable (* _Nullable MELSpriteLoader)(MELSpriteDefinition * _Nonnull self, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

typedef struct melspritedefinition {
    SpriteName name;
    MELSpriteType type;
    MELSpriteConstructor constructor;
    MELSpriteLoader loader;
    MELSize size;
    LCDBitmapTable * _Nullable palette;
    /// Taille MELAnimationDirectionCount * AnimationNameCount
    MELAnimationDefinition * _Nullable * _Nonnull animations;
} MELSpriteDefinition;

MELAnimationDefinition * _Nullable MELSpriteDefinitionGetAnimationDefinition(MELSpriteDefinition self, unsigned int animationName, MELAnimationDirection direction);

MELAnimation * _Nullable MELSpriteDefinitionGetAnimation(MELSpriteDefinition self, unsigned int animationName, MELAnimationDirection direction);

void MELSpriteDefinitionFreePalette(MELSpriteDefinition * _Nonnull self);

#endif /* constspritedefinition_h */
