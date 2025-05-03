//
//  shootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#include "shootingstyle.h"

#include "burstshootingstyle.h"
#include "circularshootingstyle.h"
#include "simpleshootingstyle.h"
#include "particuleshootingstyle.h"
#include "sprite.h"
#include "melmath.h"

extern float DELTA;

static MELPoint shotOrigin(MELShotOrigin origin, MELRectangle frame, float angle) {
    switch (origin) {
        case MELShotOriginFront:
            return MELRectangleOriginIsCenterGetPointAtAngle(frame, angle);
        case MELShotOriginCenter:
            return frame.origin;
        case MELShotOriginBack:
            return MELRectangleOriginIsCenterGetPointAtAngle(frame, angle + MEL_PI);
        default:
            playdate->system->error("Bad shot origin: %d", origin);
            return frame.origin;
    }
}

#if ENABLE_SHOOTING_STYLE_INVERSIONS
static void invert(MELShootingStyle * _Nonnull self, const MELShootingStyleInversion inversions) {
    if (inversions & MELShootingStyleInversionAmount) {
        self->bulletAmountVariation = -self->bulletAmountVariation;
    }
}
#endif

void MELShootingStyleShootFromSprite(MELShootingStyle * _Nonnull self, MELSprite * _Nonnull sprite, float angle) {
    const MELTimeInterval delta = DELTA;
    MELTimeInterval shootInterval = self->shootInterval;
    if (shootInterval > 0) {
        shootInterval -= delta;
    } else {
        const MELShootingStyleDefinition *definition = self->definition;
        shootInterval += definition->shootInterval;

        MELPoint origin = shotOrigin(definition->origin, sprite->frame, angle);
        const MELPoint translation = definition->translation;
        origin = (MELPoint) {
            .x = origin.x + translation.x,
            .y = origin.y + translation.y
        };

        // Salve de tir
        // TODO: Calculer le décalage initial des tirs en fonction de la valeur de shootInterval.
        self->class->createBullets(self, origin, angle);

        self->bulletAmount += definition->bulletAmountVariation;

#if ENABLE_SHOOTING_STYLE_INVERSIONS
        const MELShootingStyleInversion inversions = definition->inversions;
        if (inversions && self->inversionInterval > 0) {
            self->inversionInterval--;
        } else if (inversions) {
            self->inversionInterval = definition->inversionInterval;
            invert(self, inversions);
        }
#endif
    }
    self->shootInterval = shootInterval;
}

void MELShootingStyleSave(MELShootingStyle * _Nonnull self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteByte(outputStream, self->class->name);

    MELOutputStreamWriteFloat(outputStream, self->shootInterval);
    MELOutputStreamWriteUInt32(outputStream, self->bulletAmount);
    MELOutputStreamWriteUInt32(outputStream, self->bulletAmountVariation);
    MELOutputStreamWriteUInt32(outputStream, self->inversionInterval);
    MELOutputStreamWriteFloat(outputStream, self->baseAngle);
    MELOutputStreamWriteBoolean(outputStream, self->canShootWhenHitPointsAreZero);
}

MELShootingStyle MELShootingStyleLoad(MELInputStream * _Nonnull inputStream, const MELShootingStyleDefinition * _Nonnull definition) {
    MELShootingStyleClassName className = MELInputStreamReadByte(inputStream);

    const float shootInterval = MELInputStreamReadFloat(inputStream);
    const uint32_t bulletAmount = MELInputStreamReadUInt32(inputStream);
    const uint32_t bulletAmountVariation = MELInputStreamReadUInt32(inputStream);
    const uint32_t inversionInterval = MELInputStreamReadUInt32(inputStream);
    const float baseAngle = MELInputStreamReadFloat(inputStream);
    const MELBoolean canShootWhenHitPointsAreZero = MELInputStreamReadBoolean(inputStream);

    return (MELShootingStyle) {
        .class = MELShootingStyleClassForName(className),
        .definition = definition,
        .shootInterval = shootInterval,
        .bulletAmount = bulletAmount,
        .bulletAmountVariation = bulletAmountVariation,
        .inversionInterval = inversionInterval,
        .baseAngle = baseAngle,
        .canShootWhenHitPointsAreZero = canShootWhenHitPointsAreZero,
    };
}

const MELShootingStyleClass * _Nullable MELShootingStyleClassForName(MELShootingStyleClassName className) {
    switch (className) {
        case MELShootingStyleClassNameBurst:
            return BurstShootingStyleGetClass();
        case MELShootingStyleClassNameCircular:
            return CircularShootingStyleGetClass();
        case MELShootingStyleClassNameSimple:
            return SimpleShootingStyleGetClass();
        case MELShootingStyleClassNameParticule:
            return ParticuleShootingStyleGetClass();
        default:
            playdate->system->error("Unsupported shooting style class: %d", className);
            return NULL;
    }
}
