//
//  gamescene.h
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#ifndef gamescene_h
#define gamescene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
} GameScene;

GameScene * _Nonnull GameSceneAlloc(void);

#endif /* gamescene_h */
