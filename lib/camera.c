//
//  camera.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#include "camera.h"

#include "sprite.h"
#include "melmath.h"
#include "random.h"

MELCamera camera = (MELCamera) {
    .frame = {
        .size = {
            .width = LCD_COLUMNS,
            .height = LCD_ROWS
        }
    },
};

#pragma mark - Tremblement de terre

static void shakeSave(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
static MELSprite * _Nullable shakeLoad(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);

typedef struct {
    MELSprite super;
    float time;
    float duration;
    float intensity;
    MELPoint oldTranslation;
} MELCameraShake;

static const MELSpriteClass MELCameraShakeClass = (MELSpriteClass) {
    .destroy = MELSpriteDealloc,
    .save = shakeSave,
    .load = shakeLoad,
};

const MELSpriteClass * _Nonnull MELCameraShakeGetClass(void) {
    return &MELCameraShakeClass;
}

static void updateShaking(LCDSprite * _Nonnull sprite) {
    MELCameraShake *self = playdate->sprite->getUserdata(sprite);
    const float time = self->time += DELTA;
    const float progress = MELEaseInOut(0, self->duration, time);
    const float intensity = (1.0f - progress) * self->intensity;
    MELPoint oldTranslation = self->oldTranslation;
    MELPoint translation = (MELPoint) {
        .x = MELRandomFloat(intensity) - intensity / 2.0f,
        .y = MELRandomFloat(intensity) - intensity / 2.0f
    };
    self->oldTranslation = translation;
    camera.frame.origin.x += translation.x - oldTranslation.x;
    camera.frame.origin.y += translation.y - oldTranslation.y;
    if (progress == 1.0f) {
        MELSpriteDealloc(sprite);
    }
}

LCDSprite * _Nonnull MELCameraShakeConstructor(float duration, float intensity) {
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUpdateFunction(sprite, updateShaking);
    playdate->sprite->setDrawFunction(sprite, MELSpriteNoopDraw);
    playdate->sprite->setVisible(sprite, false);
    playdate->sprite->addSprite(sprite);

    MELCameraShake *self = playdate->system->realloc(NULL, sizeof(MELCameraShake));
    *self = (MELCameraShake) {
        .super = {
            .class = &MELSpriteClassDefault,
        },
        .duration = duration,
        .intensity = intensity,
    };
    playdate->sprite->setUserdata(sprite, self);
    return sprite;
}

static void shakeSave(MELSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream) {
    MELCameraShake *self = (MELCameraShake *)sprite;
    MELOutputStreamWriteFloat(outputStream, self->time);
    MELOutputStreamWriteFloat(outputStream, self->duration);
    MELOutputStreamWriteFloat(outputStream, self->intensity);
    MELOutputStreamWritePoint(outputStream, self->oldTranslation);
}

static MELSprite * _Nullable shakeLoad(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream) {
    playdate->sprite->setUpdateFunction(sprite, updateShaking);

    const float time = MELInputStreamReadFloat(inputStream);
    const float duration = MELInputStreamReadFloat(inputStream);
    const float intensity = MELInputStreamReadFloat(inputStream);
    const MELPoint oldTranslation = MELInputStreamReadPoint(inputStream);

    MELCameraShake *self = playdate->system->realloc(NULL, sizeof(MELCameraShake));
    *self = (MELCameraShake) {
        .super = {
            .class = &MELCameraShakeClass,
        },
        .time = time,
        .duration = duration,
        .intensity = intensity,
        .oldTranslation = oldTranslation,
    };
    return &self->super;
}
