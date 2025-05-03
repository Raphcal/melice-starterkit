//
//  simpleshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/03/2023.
//

#include "simpleshootingstyle.h"

#include "bullet.h"
#include "melmath.h"
#include "random.h"

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const MELShootingStyleClass SimpleShootingStyleClass = (MELShootingStyleClass) {
    .name = MELShootingStyleClassNameSimple,
    .createBullets = createBullets,
};

const MELShootingStyleClass * _Nonnull SimpleShootingStyleGetClass(void) {
    return &SimpleShootingStyleClass;
}

void SimpleShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition) {
    *self = (MELShootingStyle) {
        .class = &SimpleShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
    };
}

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    const MELShootingStyleDefinition *definition = self->definition;
    const float bulletSpeed = definition->bulletSpeed;
    BulletConstructor(definition, origin, (MELPoint) {
        .x = bulletSpeed * cosf(angle),
        .y = bulletSpeed * sinf(angle)
    });
}
