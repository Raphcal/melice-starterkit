//
//  titlescene.c
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#include "titlescene.h"

static void init(MELScene * _Nonnull scene);
static void dealloc(MELScene * _Nonnull scene);
static int update(void * _Nonnull userdata);

TitleScene * _Nonnull TitleSceneAlloc(void) {
    TitleScene *self = new(TitleScene);
    *self = (TitleScene) {
        .super = {
            .type = SceneTypeTitle,
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
    if (scene->type != SceneTypeTitle) {
        playdate->system->error("Given scene is not a TitleScene instance");
        return;
    }
    TitleScene *self = (TitleScene *)scene;
    playdate->system->realloc(self, 0);;
}

static int update(void * _Nonnull userdata) {
    return true;
}
