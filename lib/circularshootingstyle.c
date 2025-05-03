//
//  circularshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/02/2023.
//

#include "circularshootingstyle.h"

#include "bullet.h"
#include "melmath.h"
#include "random.h"

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const MELShootingStyleClass CircularShootingStyleClass = (MELShootingStyleClass) {
    .name = MELShootingStyleClassNameCircular,
    .createBullets = createBullets,
};

const MELShootingStyleClass * _Nonnull CircularShootingStyleGetClass(void) {
    return &CircularShootingStyleClass;
}

void CircularShootingStyleInit(MELShootingStyle * _Nonnull self, const CircularShootingStyleDefinition * _Nonnull definition) {
    *self = (MELShootingStyle) {
        .class = &CircularShootingStyleClass,
        .definition = &definition->super,
        .shootInterval = MELRandomFloat(definition->super.shootInterval),
        .bulletAmount = definition->super.bulletAmount,
        .bulletAmountVariation = definition->super.bulletAmountVariation,
        .inversionInterval = definition->super.inversionInterval,
        .baseAngle = definition->baseAngle,
    };
}

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    CircularShootingStyleDefinition *definition = (CircularShootingStyleDefinition *) self->definition;

    const float bulletSpeed = definition->super.bulletSpeed;
    const float baseAngle = self->baseAngle;
    self->baseAngle = baseAngle + definition->baseAngleVariation;
    angle += baseAngle;

    const unsigned int bulletAmount = self->bulletAmount;
    float angleIncrement = definition->angleIncrement;
    if (!angleIncrement) {
        angleIncrement = MEL_2_PI / bulletAmount;
    }

    for (unsigned int index = 0; index < bulletAmount; index++) {
        BulletConstructor(&definition->super, origin, (MELPoint) {
            .x = cosf(angle) * bulletSpeed,
            .y = sinf(angle) * bulletSpeed,
        });
        angle += angleIncrement;
    }
}
