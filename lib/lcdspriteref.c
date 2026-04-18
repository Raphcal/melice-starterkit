//
//  lcdspriteref.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "lcdspriteref.h"

#include "sprite.h"

MELListImplement(LCDSpriteRef);
MELListImplementIndexOf(LCDSpriteRef);

void LCDSpriteRefListDeallocReverse(LCDSpriteRefList * _Nonnull self) {
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("LCDSpriteRefListDeallocReverse");
#endif
    const LCDSpriteRefList sprites = *self;
    for (int index = sprites.count - 1; index >= 0; index--) {
        LCDSprite *sprite = sprites.memory[index];
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        playdate->system->logToConsole("Destroy %x", sprite);
#endif
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        playdate->system->logToConsole("- melSprite: %x, name: %d, type: %d", melSprite, melSprite != NULL ? melSprite->definition.type : 0, melSprite != NULL ? melSprite->definition.name : 0);
#endif
        melSprite->class->destroy(sprite);
    }
    LCDSpriteRefListDeinit(self);
}

void LCDSpriteDeinit(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("LCDSpriteDeinit(%x, %x): %d", sprite, self, self->definition.name);
#endif
    self->class->destroy(sprite);
}

void LCDSpriteRefDeinit(LCDSpriteRef * _Nonnull spriteRef) {
    LCDSpriteDeinit(*spriteRef);
    *spriteRef = NULL;
}
