//
//  scene.c
//  Roll
//
//  Created by Raphaël Calabro on 12/12/2022.
//

#include "scene.h"

#include "../src/titlescene.h"
#include "../src/gamescene.h"

MELScene * _Nullable currentScene;
PDMenuItem * _Nullable backToTitleMenuItem;
PDMenuItem * _Nullable retryMenuItem;

static void backToTitle(void * _Nullable userdata) {
    TitleScene *titleScene = TitleSceneAlloc();
    MELSceneMakeCurrent(&titleScene->super);
}

static void retry(void * _Nullable userdata) {
    GameScene *current = GameSceneGetCurrent();
    GameScene *newGame = GameSceneAlloc();
    MELSceneMakeCurrent(&newGame->super);
}

void MELSceneMakeCurrent(MELScene * _Nonnull self) {
    if (currentScene != NULL) {
        currentScene->dealloc(currentScene);
        if (playdate->sprite->getSpriteCount() > 0) {
            playdate->system->logToConsole("%d sprites remaining, calling update to dealloc properly", playdate->sprite->getSpriteCount());
            playdate->sprite->updateAndDrawSprites();
        }
        playdate->sprite->removeAllSprites();
    }
    currentScene = self;
    playdate->system->setUpdateCallback(self->update, self);
    self->init(self);

    MELSceneAddOrRemoveBackToTitleMenuItem();
    playdate->system->resetElapsedTime();
}

void MELSceneAddOrRemoveBackToTitleMenuItem(void) {
    if (currentScene->type == SceneTypeGame && !backToTitleMenuItem) {
        backToTitleMenuItem = playdate->system->addMenuItem("Back to title", backToTitle, NULL);
        retryMenuItem = playdate->system->addMenuItem("Retry", retry, NULL);
    } else if (currentScene->type != SceneTypeGame && backToTitleMenuItem) {
        playdate->system->removeMenuItem(backToTitleMenuItem);
        playdate->system->removeMenuItem(retryMenuItem);
        backToTitleMenuItem = NULL;
        retryMenuItem = NULL;
    }
}

LCDSprite * _Nullable MELSceneFindSpriteByName(SpriteName spriteName) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == spriteName) {
            return sprite;
        }
    }
    playdate->system->logToConsole("No sprite found in current scene for name: %d", spriteName);
    return NULL;
}

LCDSprite * _Nullable MELSceneFindSpriteByClassName(SpriteClassName className) {
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->class->name == className) {
            return sprite;
        }
    }
    playdate->system->logToConsole("No sprite found in current scene for class name: %d", className);
    return NULL;
}

unsigned int MELSceneFindSpritesByName(SpriteName spriteName, LCDSprite * _Nonnull * _Nonnull result, int max) {
    unsigned int resultIndex = 0;
    LCDSpriteRefList sprites = currentScene->sprites;
    for (unsigned int index = 0; index < sprites.count && resultIndex < max; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        if (melSprite->definition.name == spriteName) {
            result[resultIndex++] = sprite;
        }
    }
    return resultIndex;
}
