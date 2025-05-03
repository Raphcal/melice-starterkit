#include "stride.h"

typedef struct {
    void * _Nullable oldUserdata;
    float time;
    float delay;
    float duration;
    MELPoint origin;
    MELSize distance;
    MELEasingFunction easingFunction;
    MELBoolean destroyWhenStrideEnds;
    MELBoolean oldAutoReleaseUserdata;
} Stride;

static void updateAndFillBlack(LCDSprite * _Nonnull sprite);

static void (* _Nonnull getUpdateFunction(Stride * _Nonnull self, const MELSpriteClass * _Nonnull class))(LCDSprite * _Nonnull);

void StrideSpriteFromAndTo(LCDSprite * _Nonnull sprite, MELPoint from, MELPoint to, float delay, float duration) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    self->frame.origin = from;
    StrideSpriteTo(sprite, to, delay, duration);
}

void StrideSpriteTo(LCDSprite * _Nonnull sprite, MELPoint to, float delay, float duration) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->userdata && self->autoReleaseUserdata) {
        playdate->system->realloc(self->userdata, 0);
        self->userdata = NULL;
    }
    Stride *stride = playdate->system->realloc(NULL, sizeof(Stride));
    MELPoint from = self->frame.origin;
    *stride = (Stride) {
        .origin = self->frame.origin,
        .distance = {
            .width = to.x - from.x,
            .height = to.y - from.y
        },
        .delay = delay,
        .duration = delay + duration,
        .easingFunction = MELEaseInOut,
        .oldUserdata = self->userdata,
        .oldAutoReleaseUserdata = self->autoReleaseUserdata,
    };
    self->userdata = stride;
    self->autoReleaseUserdata = true;
    playdate->sprite->setUpdateFunction(sprite, StrideUpdate);
}

void StrideSpriteBy(LCDSprite * _Nonnull sprite, MELPoint translation, float delay, float duration) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    StrideSpriteTo(sprite, MELPointAdd(self->frame.origin, translation), delay, duration);
}

LCDSprite * _Nonnull StrideConstructor(MELSpriteDefinition * _Nonnull definition, MELPoint from, MELPoint to, float delay, float duration) {
    MELSprite *self = playdate->system->realloc(NULL, sizeof(MELSprite));
    LCDSprite *sprite = MELSpriteInitWithCenter(self, definition, from);
    StrideSpriteTo(sprite, to, delay, duration);

#if LOG_SPRITE_PUSH_AND_REMOVE_FROM_SCENE_SPRITES
    playdate->system->logToConsole("Push StrideConstructor(%x, %x): %d", sprite, self, self->definition.name);
#endif
    return sprite;
}

void StrideTogetherAlignedRight(LCDSprite * _Nullable sprite, LCDSprite * _Nullable spriteToFollow) {
    if (!sprite || !spriteToFollow) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELSprite *other = playdate->sprite->getUserdata(spriteToFollow);

    if (self->userdata == NULL) {
        self->userdata = playdate->system->realloc(NULL, sizeof(Stride));
        self->autoReleaseUserdata = true;
    }
    Stride *selfStride = self->userdata;
    Stride *otherStride = other->userdata;

    MELRectangle selfFrame = self->frame;
    MELRectangle otherFrame = other->frame;
    self->frame = (MELRectangle) {
        .origin = {
            .x = otherFrame.origin.x + otherFrame.size.width / 2.0f + selfFrame.size.width / 2.0f,
            .y = otherFrame.origin.y
        }
    };
    *selfStride = *otherStride;
    playdate->sprite->setUpdateFunction(sprite, StrideUpdate);
}

void StrideSkip(LCDSprite * _Nullable sprite) {
    if (StrideIsDone(sprite)) {
        return;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    Stride *stride = self->userdata;
    MELRectangle frame = self->frame;
    frame.origin = (MELPoint) {
        .x = stride->origin.x + stride->distance.width,
        .y = stride->origin.y + stride->distance.height,
    };
    self->frame = frame;
    self->userdata = stride->oldUserdata;
    self->autoReleaseUserdata = stride->oldAutoReleaseUserdata;

    const MELSpritePositionFixed fixed = self->fixed;
    const MELPoint origin = frame.origin;
    const float x = (fixed & MELSpritePositionFixedX) ? origin.x : origin.x - camera.frame.origin.x;
    const float y = (fixed & MELSpritePositionFixedY) ? origin.y : origin.y - camera.frame.origin.y;
    playdate->sprite->moveTo(sprite, x, y);
    playdate->sprite->setUpdateFunction(sprite, getUpdateFunction(stride, self->class));

    playdate->system->realloc(stride, 0);
}

void StrideResume(LCDSprite * _Nonnull sprite) {
    playdate->sprite->setUpdateFunction(sprite, StrideUpdate);
}

MELBoolean StrideIsDone(LCDSprite * _Nullable sprite) {
    if (!sprite) {
        return true;
    }
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    return self->userdata == NULL;
}

void StrideSetEasingFunction(LCDSprite * _Nonnull sprite, MELEasingFunction easingFunction) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    Stride *stride = self->userdata;
    stride->easingFunction = easingFunction;
}

void StrideSetProgress(LCDSprite * _Nonnull sprite, float progress) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    Stride *stride = self->userdata;
    stride->time = stride->duration * progress;
}

void StrideSetFillBlack(LCDSprite * _Nonnull sprite) {
    playdate->sprite->setUpdateFunction(sprite, updateAndFillBlack);
}

void StrideSetDestroyWhenStrideEnds(LCDSprite * _Nonnull sprite, MELBoolean destroyWhenStrideEnds) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    if (self->userdata) {
        Stride *stride = self->userdata;
        stride->destroyWhenStrideEnds = destroyWhenStrideEnds;
    }
}

void StrideUpdate(LCDSprite * _Nonnull sprite) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);

    MELAnimation *animation = self->animation;
    if (animation) {
        MELAnimationUpdate(animation, DELTA);
        playdate->sprite->setImage(sprite, playdate->graphics->getTableBitmap(self->definition.palette, animation->frame.atlasIndex), MELDirectionFlip[self->direction]);
    }

    Stride *stride = self->userdata;
    if (!stride) {
        return;
    }
    MELRectangle frame = self->frame;
    const float duration = stride->duration;
    float time = stride->time;
    if (time < duration) {
        stride->time = time = MELFloatMin(time + DELTA, duration);
        const float progress = stride->easingFunction(stride->delay, duration, time);

        frame.origin.x = stride->origin.x + stride->distance.width * progress;
        frame.origin.y = stride->origin.y + stride->distance.height * progress;
        self->frame = frame;

        const MELSpritePositionFixed fixed = self->fixed;
        const MELPoint origin = frame.origin;
        const float x = (fixed & MELSpritePositionFixedX) ? origin.x : origin.x - camera.frame.origin.x;
        const float y = (fixed & MELSpritePositionFixedY) ? origin.y : origin.y - camera.frame.origin.y;
        playdate->sprite->moveTo(sprite, x, y);
        return;
    }
    StrideSkip(sprite);
}

static void updateAndFillBlack(LCDSprite * _Nonnull sprite) {
    StrideUpdate(sprite);
    playdate->sprite->setDrawMode(sprite, kDrawModeFillBlack);
}

static void (* _Nonnull getUpdateFunction(Stride * _Nonnull self, const MELSpriteClass * _Nonnull class))(LCDSprite * _Nonnull) {
    if (self->destroyWhenStrideEnds) {
        return class->destroy;
    } else if (class->update) {
        return class->update;
    } else {
        return MELSpriteUpdate;
    }
}
