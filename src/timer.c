//
//  timer.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 04/03/2025.
//

#include "timer.h"

typedef struct {
    MELSprite super;
    float delay;
    float time;
    TimerCallback callback;
} Timer;

static void update(LCDSprite * _Nonnull sprite);

LCDSprite * _Nonnull TimerConstructor(float delay, TimerCallback callback, void * _Nullable userdata) {
    Timer *self = new(Timer);

    *self = (Timer) {
        .super = {
            .class = &MELSpriteClassDefault,
            .userdata = userdata,
        },
        .delay = delay,
        .callback = callback,
    };

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, &update);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->addSprite(sprite);

    LCDSpriteRefListPush(&currentScene->sprites, sprite);
    return sprite;
}

static void update(LCDSprite * _Nonnull sprite) {
    Timer *self = playdate->sprite->getUserdata(sprite);
    const float duration = self->delay;
    if (self->time < duration) {
        self->time += DELTA;
        return;
    }
    self->callback(self->super.userdata);
    MELSpriteDealloc(sprite);
}
