//
//  spriteinstance.h
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef spriteinstance_h
#define spriteinstance_h

#include "melstd.h"

#include "point.h"
#include "operation.h"
#include "inputstream.h"
#include "list.h"
#include "direction.h"

#include "../gen/spritenames.h"
#include "../gen/maps.h"

typedef struct melmap MELMap;
typedef struct mellayer MELLayer;

typedef struct melspriteinstance {
    SpriteName name;
    MELPoint center;
    MELDirection direction;
    int8_t zIndex;
    MELBoolean isUnique;
    /// Active sprites are not unloaded when they are far from the player.
    MELBoolean active;
    /// Destroyed sprites won't be created again.
    MELBoolean destroyed;
    int32_t variableCount;
    int32_t * _Nullable variables;
    MELMap * _Nullable map;
    MapName mapName;
    LCDSprite * _Nullable sprite;
} MELSpriteInstance;

MELListDefine(MELSpriteInstance);

MELSpriteInstance MELSpriteInstanceMake(uint16_t definitionIndex, MELPoint center, MELDirection direction, MELBoolean isUnique);
MELSpriteInstance MELSpriteInstanceMakeWithSpriteInstance(MELSpriteInstance other);
MELSpriteInstance MELSpriteInstanceMakeWithInputStream(MELInputStream * _Nonnull inputStream);

void MELSpriteInstanceDeinit(MELSpriteInstance * _Nonnull self);

MELLayer * _Nullable MELSpriteInstanceGetGround(MELSpriteInstance * _Nonnull self);

int32_t MELSpriteInstanceGetVariableOrDefault(MELSpriteInstance * _Nonnull self, int variable, int32_t defaultValue);

#endif /* spriteinstance_h */
