//
//  timer.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 04/03/2025.
//

#include "timer.h"

#include "sprite.h"
#include "scene.h"

typedef struct {
    MELSprite super;
    float delay;
    float time;
    MELTimerCallback callback;
} MELTimer;

static void update(LCDSprite * _Nonnull sprite);

LCDSprite * _Nonnull MELTimerConstructor(float delay, MELTimerCallback callback, void * _Nullable userdata) {
    return MELTimerConstructorWithAutoRelease(delay, callback, userdata, false);
}

LCDSprite * _Nonnull MELTimerConstructorWithAutoRelease(float delay, MELTimerCallback callback, void * _Nullable userdata, MELBoolean autoReleaseUserdata) {
    MELTimer *self = new(MELTimer);

    *self = (MELTimer) {
        .super = {
            .class = &MELSpriteClassDefault,
            .userdata = userdata,
            .autoReleaseUserdata = autoReleaseUserdata,
        },
        .delay = delay,
        .callback = callback,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->addSprite(sprite);

    MELSceneAddSprite(sprite);
    return sprite;
}

static void update(LCDSprite * _Nonnull sprite) {
    MELTimer *self = playdate->sprite->getUserdata(sprite);
    const float duration = self->delay;
    if (self->time < duration) {
        self->time += DELTA;
        return;
    }
    self->callback(self->super.userdata);
    MELSpriteDealloc(sprite);
}
