//
//  fadetoblackscene.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 04/08/2024.
//

#include "fadetoblackscene.h"

#include "primitives.h"
#include "melmath.h"
#include "bitmap.h"

typedef struct {
    MELFade super;
    LCDBitmap * _Nullable pattern;
    LCDBitmap * _Nullable fade;
    float time;
    uint8_t opacity;
} MELFadeToBlackScene;

static const float kFadeDuration = 0.1f;

static void dealloc(MELScene * _Nonnull scene);
static void init(MELScene * _Nonnull scene);
static int updateFadeIn(void * _Nonnull userdata);
static int updateFadeOut(void * _Nonnull userdata);

MELScene * _Nonnull MELFadeToBlackSceneAlloc(MELScene * _Nonnull oldScene, MELScene * _Nonnull nextScene) {
    MELFadeToBlackScene *self = new(MELFadeToBlackScene);
    *self = (MELFadeToBlackScene) {
        .super = {
            .super = {
                .type = SceneTypeFade,
                .init = init,
                .dealloc = dealloc,
                .update = updateFadeIn,
            },
            .oldScene = oldScene,
            .nextScene = nextScene,
        },
    };
    return &self->super.super;
}

static void init(MELScene * _Nonnull scene) {
    MELFadeToBlackScene *self = (MELFadeToBlackScene *)scene;
    self->pattern = playdate->graphics->newBitmap(8, 8, kColorClear);
    self->fade = playdate->graphics->newBitmap(LCD_COLUMNS, LCD_ROWS, kColorClear);
}

static void dealloc(MELScene * _Nonnull scene) {
    if (scene->type != SceneTypeFade) {
        playdate->system->error("Given scene is not a MELFadeToBlackScene instance");
        return;
    }
    MELFadeToBlackScene *self = (MELFadeToBlackScene *)scene;
    if (self->pattern) {
        playdate->graphics->freeBitmap(self->pattern);
        self->pattern = NULL;
    }
    if (self->fade) {
        playdate->graphics->freeBitmap(self->fade);
        self->fade = NULL;
    }
    if (self->super.oldScene) {
        self->super.oldScene->dealloc(self->super.oldScene);
        self->super.oldScene = NULL;
    }
    if (self->super.nextScene) {
        self->super.nextScene->dealloc(self->super.nextScene);
        self->super.nextScene = NULL;
    }
    playdate->system->realloc(self, 0);
}

static void setOpacity(MELFadeToBlackScene * _Nonnull self, float opacity) {
    const uint8_t opacityAsUint8 = (uint8_t) MELFloatBound(0.0f, opacity * 100.0f, 100.0f);
    if (opacityAsUint8 == self->opacity) {
        return;
    }
    self->opacity = opacityAsUint8;

    LCDBitmap *pattern = self->pattern;
    playdate->graphics->clearBitmap(pattern, kColorClear);
    LCDBitmapFadeImage(pattern, opacityAsUint8);

    LCDBitmap *fade = self->fade;
    playdate->graphics->clearBitmap(fade, kColorClear);
    playdate->graphics->pushContext(fade);
    playdate->graphics->tileBitmap(pattern, 0, 0, LCD_COLUMNS, LCD_ROWS, kBitmapUnflipped);
    playdate->graphics->popContext();
}

static int updateFadeIn(void * _Nonnull userdata) {
    MELFadeToBlackScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    float time = self->time;
    if (time < kFadeDuration) {
        time += DELTA;
        self->time = time;
        const float progress = MELEaseIn(0.0f, kFadeDuration, time);
        setOpacity(self, progress);
        playdate->graphics->drawBitmap(self->fade, 0, 0, kBitmapUnflipped);
        return true;
    }

    currentScene = self->super.oldScene;
    self->super.oldScene->dealloc(self->super.oldScene);
    self->super.oldScene = NULL;

    currentScene = self->super.nextScene;
    self->super.nextScene->init(self->super.nextScene);

    setOpacity(self, 1.0f);
    playdate->graphics->drawBitmap(self->fade, 0, 0, kBitmapUnflipped);

    self->time = 0.0f;
    playdate->system->setUpdateCallback(updateFadeOut, self);
    return true;
}

static int updateFadeOut(void * _Nonnull userdata) {
    MELFadeToBlackScene *self = userdata;

    DELTA = playdate->system->getElapsedTime();
    playdate->system->resetElapsedTime();

    playdate->sprite->updateAndDrawSprites();

    float time = self->time;
    if (time < kFadeDuration) {
        time += DELTA;
        self->time = time;
        const float progress = MELEaseIn(0.0f, kFadeDuration, time);
        setOpacity(self, 1.0f - progress);
        playdate->graphics->drawBitmap(self->fade, 0, 0, kBitmapUnflipped);
        return true;
    }

    setOpacity(self, 0.0f);
    playdate->graphics->drawBitmap(self->fade, 0, 0, kBitmapUnflipped);

    currentScene = self->super.nextScene;
    self->super.nextScene = NULL;
    playdate->system->setUpdateCallback(currentScene->update, currentScene);
    self->super.super.dealloc(&self->super.super);
    return true;
}
