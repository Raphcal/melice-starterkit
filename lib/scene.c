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
    if (currentScene && currentScene->beforeQuit) {
        currentScene->beforeQuit(currentScene);
    }
    TitleScene *titleScene = TitleSceneAlloc();
    MELSceneMakeCurrent(&titleScene->super);
}

void MELSceneMakeCurrent(MELScene * _Nonnull self) {
    if (currentScene != NULL) {
        currentScene->dealloc(currentScene);
        if (playdate->sprite->getSpriteCount() > 0) {
            playdate->system->logToConsole("%d sprites remaining, calling update to dealloc properly", playdate->sprite->getSpriteCount());
            MELScene emptyScene = (MELScene) {};
            currentScene = &emptyScene;
            playdate->sprite->updateAndDrawSprites();
        }
        if (playdate->sprite->getSpriteCount() > 0) {
            playdate->system->logToConsole("Still %d sprites remaining!", playdate->sprite->getSpriteCount());
        }
    }
    currentScene = self;
    playdate->system->setUpdateCallback(self->update, self);
    self->init(self);
    playdate->system->resetElapsedTime();

    MELSceneAddOrRemoveBackToTitleMenuItem();
}

MELScene * _Nonnull MELSceneGetCurrent(void) {
    MELScene *scene = currentScene;
    if (scene->type == SceneTypeFade) {
        MELFade *fade = (MELFade *)scene;
        if (fade->oldScene) {
            scene = fade->oldScene;
        } else if (fade->nextScene) {
            scene = fade->nextScene;
        }
    }
    return scene;
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
    return sprite;
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
    return sprite;
}

void MELSceneFadeTo(MELScene * _Nonnull nextScene, MELScene * _Nonnull (* _Nonnull fadeConstructor)(MELScene * _Nonnull oldScene, MELScene * _Nonnull nextScene)) {
    MELScene *fade = fadeConstructor(currentScene, nextScene);
    currentScene = fade;
    fade->init(fade);
    playdate->system->setUpdateCallback(fade->update, fade);
}

void MELSceneAddSprite(LCDSprite * _Nonnull sprite) {
    if (currentScene->addSprite) {
        currentScene->addSprite(currentScene, sprite);
    } else {
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        playdate->system->logToConsole("MELSceneAddSprite push sprite %x, name: %d, type: %d", sprite, melSprite != NULL ? melSprite->definition.type : 0, melSprite != NULL ? melSprite->definition.name : 0);
#endif
#if DEBUG
        const LCDSpriteRefList sprites = currentScene->sprites;
        for (unsigned int index = 0; index < sprites.count; index++) {
            if (sprites.memory[index] == sprite) {
                playdate->system->error("MELSceneAddSprite error: trying to add sprite %x twice!", sprite);
            }
        }
#endif
        LCDSpriteRefListPush(&currentScene->sprites, sprite);
    }
}

void MELFadeAddSprite(MELScene * _Nonnull scene, LCDSprite * _Nonnull sprite) {
    MELFade *self = (MELFade *)scene;
    if (self->oldScene != NULL) {
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        playdate->system->logToConsole("MELFadeAddSprite add sprite %x to oldScene, name: %d, type: %d", sprite, melSprite != NULL ? melSprite->definition.type : 0, melSprite != NULL ? melSprite->definition.name : 0);
#endif
        LCDSpriteRefListPush(&self->oldScene->sprites, sprite);
    } else if (self->nextScene != NULL) {
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        playdate->system->logToConsole("MELFadeAddSprite add sprite %x to nextScene, name: %d, type: %d", sprite, melSprite != NULL ? melSprite->definition.type : 0, melSprite != NULL ? melSprite->definition.name : 0);
#endif
        LCDSpriteRefListPush(&self->nextScene->sprites, sprite);
    } else {
#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
        MELSprite *melSprite = playdate->sprite->getUserdata(sprite);
        playdate->system->logToConsole("MELFadeAddSprite add sprite %x to fade, name: %d, type: %d", sprite, melSprite != NULL ? melSprite->definition.type : 0, melSprite != NULL ? melSprite->definition.name : 0);
#endif
        LCDSpriteRefListPush(&self->super.sprites, sprite);
    }
}
