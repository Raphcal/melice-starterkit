//
//  gamescene.c
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#include "gamescene.h"

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

GameScene * _Nonnull GameSceneAlloc(void) {
    GameScene *self = new(GameScene);
    *self = (GameScene) {
        .super = {
            .type = SceneTypeGame,
            .init = init,
            .dealloc = dealloc,
            .update = update,
        },
    };
    return self;
}

static void init(MELScene * _Nonnull scene) {
    // TODO: Initialisation.
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeGame) {
        playdate->system->error("Given scene is not a GameScene instance");
        return;
    }
    GameScene *self = (GameScene *)scene;
    playdate->system->realloc(self, 0);
}

static int update(void * _Nonnull userdata) {
    return true;
}
