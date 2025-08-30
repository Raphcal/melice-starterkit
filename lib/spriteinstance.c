//
//  spriteinstance.c
//  shmup
//
//  Created by Raphaël Calabro on 05/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "spriteinstance.h"

#include "map.h"
#include "../gen/spritevariables.h"
#include "sprite.h"

MELListImplement(MELSpriteInstance);

MELSpriteInstance MELSpriteInstanceMake(uint16_t definitionIndex, MELPoint center, MELDirection direction, MELBoolean isUnique) {
    return (MELSpriteInstance) {
        .name = (SpriteName) definitionIndex,
        .center = center,
        .direction = direction,
        .isUnique = isUnique
    };
}

MELSpriteInstance MELSpriteInstanceMakeWithSpriteInstance(MELSpriteInstance other) {
    return other;
}

MELSpriteInstance MELSpriteInstanceMakeWithInputStream(MELInputStream * _Nonnull inputStream) {
    MELSpriteInstance instance = {};
    instance.name = (SpriteName) MELInputStreamReadUInt16(inputStream);
    instance.direction = (MELDirection) MELInputStreamReadByte(inputStream);
    const int x = MELInputStreamReadInt(inputStream);
    const int y = MELInputStreamReadInt(inputStream);
    instance.center = MELPointMake(x, y);
    instance.zIndex = MELInputStreamReadByte(inputStream) & 0xFF;
    instance.isUnique = MELInputStreamReadBoolean(inputStream);
    instance.variables = MELInputStreamReadIntArray(inputStream, &instance.variableCount);
    return instance;
}

void MELSpriteInstanceDeinit(MELSpriteInstance * _Nonnull self) {
    if (self->variables) {
        playdate->system->realloc(self->variables, 0);
        self->variables = NULL;
    }
    if (self->sprite) {
        MELSprite *sprite = playdate->sprite->getUserdata(self->sprite);
        sprite->instance = NULL;
        self->sprite = NULL;
    }
    self->variableCount = 0;
}

MELLayer * _Nullable MELSpriteInstanceGetGround(MELSpriteInstance * _Nonnull self) {
    const int8_t zIndex = self->zIndex;
    MELMap *map = self->map;
    if (map == NULL) {
        return NULL;
    } else {
        return zIndex < self->map->layerCount ? self->map->layers + zIndex : self->map->grounds.memory[0];
    }
}

int32_t MELSpriteInstanceGetVariableOrDefault(MELSpriteInstance * _Nonnull self, int variable, int32_t defaultValue) {
    int32_t value = self->variables[variable];
    return value != kSpriteVariableNoValue ? value : defaultValue;
}
