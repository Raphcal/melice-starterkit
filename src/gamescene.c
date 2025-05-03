//
//  gamescene.c
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#include "gamescene.h"

GameScene * _Nonnull GameSceneAlloc(void) {
    GameScene *self = new(GameScene);
    *self = (GameScene) {
        .super = {
        },
    };
    return self;
}
