//
//  sinussimpleshootingstyle.c
//  ColdBird
//
//  Created by Raphaël Calabro on 29/01/2025.
//

#include "sinussimpleshootingstyle.h"

#include "bullet.h"
#include "random.h"

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle, float initialDelta);

static const MELShootingStyleClass SinusShootingStyleClass = (MELShootingStyleClass) {
    .name = MELShootingStyleClassNameSinus,
    .createBullets = createBullets,
};

const MELShootingStyleClass * _Nonnull SinusShootingStyleGetClass(void) {
    return &SinusShootingStyleClass;
}

void SinusShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition) {
    if (!definition->bulletDefinition->palette) {
        definition->bulletDefinition->palette = SpriteNameLoadBitmapTable(definition->bulletDefinition->name);
    }
    *self = (MELShootingStyle) {
        .class = &SinusShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
    };
}

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle, float initialDelta) {
    const MELShootingStyleDefinition *definition = self->definition;
    unsigned int time = playdate->system->getCurrentTimeMilliseconds();
    const float progress = sinf((time * definition->speeds.x) / (1000.0f));
    BulletConstructor(definition,
                      MELPointAdd(origin, MELPointMake(progress * definition->space, 0.0f)),
                      MELPointMake(0.0f, definition->speeds.y * definition->bulletSpeed),
                      initialDelta);
    BulletConstructor(definition,
                      MELPointAdd(origin, MELPointMake(-progress * definition->space, 0.0f)),
                      MELPointMake(0.0f, definition->speeds.y * definition->bulletSpeed),
                      initialDelta);
}
