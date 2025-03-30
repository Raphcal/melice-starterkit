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
    // FIXME: Création d'une copie défensive à cause de GameOverScene. Après suppression de GameOverScene, supprimer ceci.
    LCDSpriteRefList sprites = LCDSpriteRefListMakeWithList(*self);
    for (int index = sprites.count - 1; index >= 0; index--) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        playdate->system->logToConsole("Destroy %x, name: %d, type: %d", sprite, melSprite->definition.type, melSprite->definition.name);
#endif
        melSprite->class->destroy(sprite);
    }
    LCDSpriteRefListDeinit(self);
    LCDSpriteRefListDeinit(&sprites);
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
