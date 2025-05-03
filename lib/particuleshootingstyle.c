//
//  particuleshootingstyle.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/04/2023.
//

#include "particuleshootingstyle.h"

#include "../src/explosion.h"

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle);

static const MELShootingStyleClass ParticuleShootingStyleClass = (MELShootingStyleClass) {
    .name = MELShootingStyleClassNameParticule,
    .createBullets = createBullets,
};

const MELShootingStyleClass * _Nonnull ParticuleShootingStyleGetClass(void) {
    return &ParticuleShootingStyleClass;
}

void ParticuleShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition) {
    *self = (MELShootingStyle) {
        .class = &ParticuleShootingStyleClass,
        .definition = definition,
        .shootInterval = MELRandomFloat(definition->shootInterval),
        .bulletAmount = definition->bulletAmount,
        .bulletAmountVariation = definition->bulletAmountVariation,
        .inversionInterval = definition->inversionInterval,
        .canShootWhenHitPointsAreZero = true,
    };
}

static void createBullets(MELShootingStyle * _Nonnull self, MELPoint origin, float angle) {
    const MELShootingStyleDefinition *definition = self->definition;
    const float space = definition->space;
    ExplosionConstructorWithDefinition((MELPoint) {
        .x = origin.x + MELRandomFloat(space) - space / 2,
        .y = origin.y + MELRandomFloat(space) - space / 2,
    }, definition->bulletDefinition, AnimationNameStand);
}
