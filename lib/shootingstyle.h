//
//  shootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#ifndef shootingstyle_h
#define shootingstyle_h

#include "shootingstyledefinition.h"

typedef enum {
    MELShootingStyleClassNameBurst,
    MELShootingStyleClassNameCircular,
    MELShootingStyleClassNameSimple,
    MELShootingStyleClassNameParticule,
} MELShootingStyleClassName;

typedef struct {
    MELShootingStyleClassName name;
    void (* _Nonnull createBullets)(MELShootingStyle * _Nonnull self, MELPoint origin, float angle);
} MELShootingStyleClass;

const MELShootingStyleClass * _Nullable MELShootingStyleClassForName(MELShootingStyleClassName className);


typedef struct shootingstyle {
    const MELShootingStyleClass * _Nonnull class;
    const MELShootingStyleDefinition * _Nonnull definition;

    MELTimeInterval shootInterval;
    unsigned int bulletAmount;
    unsigned int bulletAmountVariation;

    unsigned int inversionInterval;

    float baseAngle;

    MELBoolean canShootWhenHitPointsAreZero;
} MELShootingStyle;

void MELShootingStyleShootFromSprite(MELShootingStyle * _Nonnull self, MELSprite * _Nonnull sprite, float angle);

void MELShootingStyleSave(MELShootingStyle * _Nonnull self, MELOutputStream * _Nonnull outputStream);
MELShootingStyle MELShootingStyleLoad(MELInputStream * _Nonnull inputStream, const MELShootingStyleDefinition * _Nonnull definition);

#endif /* shootingstyle_h */
