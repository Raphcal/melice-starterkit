//
//  scene.c
//  Roll
//
//  Created by Raphaël Calabro on 12/12/2022.
//

#include "scene.h"

#include "../src/titlescene.h"

MELScene * _Nullable currentScene;
PDMenuItem * _Nullable backToTitleMenuItem;

static void backToTitle(void * _Nullable userdata) {
    TitleScene *titleScene = TitleSceneAlloc();
    MELSceneMakeCurrent(&titleScene->super);
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
    playdate->system->resetElapsedTime();

    MELSceneAddOrRemoveBackToTitleMenuItem();
}

void MELSceneAddOrRemoveBackToTitleMenuItem(void) {
    if (!backToTitleMenuItem && currentScene->type != SceneTypeTitle) {
        backToTitleMenuItem = playdate->system->addMenuItem("Back to title", backToTitle, NULL);
    } else if (backToTitleMenuItem && currentScene->type == SceneTypeTitle) {
        playdate->system->removeMenuItem(backToTitleMenuItem);
        backToTitleMenuItem = NULL;
    }
}

static LCDSprite * _Nullable findSpriteByName(MELScene * _Nonnull self, SpriteName spriteName) {
    void *(*getUserdata)(LCDSprite*) = playdate->sprite->getUserdata;
    LCDSpriteRefList sprites = self->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = getUserdata(sprite);
        if (melSprite->definition.name == spriteName) {
            return sprite;
        }
    }
    return NULL;
}

LCDSprite * _Nullable MELSceneFindSpriteByName(SpriteName spriteName) {
    LCDSprite *sprite = findSpriteByName(currentScene, spriteName);
    if (!sprite && currentScene->type == SceneTypeFade) {
        MELFade *fade = (MELFade *)currentScene;
        if (fade->oldScene) {
            sprite = findSpriteByName(fade->oldScene, spriteName);
        }
        if (!sprite && fade->nextScene) {
            sprite = findSpriteByName(fade->nextScene, spriteName);
        }
    }
#if LOG_SPRITE_NOT_FOUND
    if (sprite == NULL) {
        playdate->system->logToConsole("No sprite found in current scene for name: %d", spriteName);
    }
#endif
    return sprite;
}

static LCDSprite * _Nullable findSpriteByClassName(MELScene * _Nonnull self, SpriteClassName className) {
    void *(*getUserdata)(LCDSprite*) = playdate->sprite->getUserdata;
    LCDSpriteRefList sprites = self->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        MELSprite *melSprite = getUserdata(sprite);
        if (melSprite->class->name == className) {
            return sprite;
        }
    }
    return NULL;
}

LCDSprite * _Nullable MELSceneFindSpriteByClassName(SpriteClassName className) {
    LCDSprite *sprite = findSpriteByClassName(currentScene, className);
    if (!sprite && currentScene->type == SceneTypeFade) {
        MELFade *fade = (MELFade *)currentScene;
        if (fade->oldScene) {
            sprite = findSpriteByClassName(fade->oldScene, className);
        }
        if (!sprite && fade->nextScene) {
            sprite = findSpriteByClassName(fade->nextScene, className);
        }
    }
#if LOG_SPRITE_NOT_FOUND
    if (sprite == NULL) {
        playdate->system->logToConsole("No sprite found in current scene for class name: %d", className);
    }
#endif
    return NULL;
}

static LCDSprite * _Nullable findSpriteByTag(MELScene * _Nonnull self, const uint8_t tag) {
    uint8_t (*getTag)(LCDSprite*) = playdate->sprite->getTag;
    LCDSpriteRefList sprites = self->sprites;
    for (unsigned int index = 0; index < sprites.count; index++) {
        LCDSprite *sprite = sprites.memory[index];
        const uint8_t spriteTag = getTag(sprite);
        if (spriteTag == tag) {
            return sprite;
        }
    }
    return NULL;
}

LCDSprite * _Nullable MELSceneFindSpriteByTag(const uint8_t tag) {
    LCDSprite *sprite = findSpriteByTag(currentScene, tag);
    if (!sprite && currentScene->type == SceneTypeFade) {
        MELFade *fade = (MELFade *)currentScene;
        if (fade->oldScene) {
            sprite = findSpriteByTag(fade->oldScene, tag);
        }
        if (!sprite && fade->nextScene) {
            sprite = findSpriteByTag(fade->nextScene, tag);
        }
    }
#if LOG_SPRITE_NOT_FOUND
    if (sprite == NULL) {
        playdate->system->logToConsole("No sprite found in current scene for tag: %d", tag);
    }
#endif
    return NULL;
}

void MELSceneFadeTo(MELScene * _Nonnull nextScene, MELScene * _Nonnull (* _Nonnull fadeConstructor)(MELScene * _Nonnull oldScene, MELScene * _Nonnull nextScene)) {
    MELScene *fade = fadeConstructor(currentScene, nextScene);
    currentScene = fade;
    fade->init(fade);
    playdate->system->setUpdateCallback(fade->update, fade);
}
