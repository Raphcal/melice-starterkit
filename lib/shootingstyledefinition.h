//
//  shootingstyledefinition.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 27/01/2023.
//

#ifndef shootingstyledefinition_h
#define shootingstyledefinition_h

#include "melstd.h"

#include "point.h"
#include "spritedefinition.h"

#include "../gen/animationnames.h"

// TODO: Retirer les inversions inutilisées à la fin.
typedef enum {
    MELShootingStyleNone            =       0,
    MELShootingStyleInversionX      =       1,
    MELShootingStyleInversionY      =    0b10,
    MELShootingStyleInversionAim    =   0b100,
    MELShootingStyleInversionAngle  =  0b1000,
    MELShootingStyleInversionAmount = 0b10000,
} MELShootingStyleInversion;

typedef enum {
    MELShotOriginCenter,
    MELShotOriginFront,
    MELShotOriginBack,
} MELShotOrigin;

typedef struct shootingstyle MELShootingStyle;
typedef struct shootingstyledefinition MELShootingStyleDefinition;

typedef struct shootingstyledefinition {
    /// Point d'origine des tirs
    MELShotOrigin origin;

    // Déplacement de l'origine du tir.
    MELPoint translation;

    /// Dommages provoqués par chaque tir
    int damage;

    /// Définition des sprites tirés.
    MELSpriteDefinition * _Nonnull bulletDefinition;

    /// Nom de l'animation à utiliser.
    AnimationName bulletAnimationName;

    /// Nombre de tirs
    int bulletAmount;
    /// Augmentation ou diminution du nombre de tirs
    int bulletAmountVariation;

    /// Vitesse d'un tir
    float bulletSpeed;
    /// Interval de temps entre chaque tir
    MELTimeInterval shootInterval;

    /// Nombre d'inversions
    MELShootingStyleInversion inversions;
    /// Nombre de tirs avant l'inversion
    int inversionInterval;

    // Espace entre les tirs.
    float space;
} MELShootingStyleDefinition;

#endif /* shootingstyledefinition_h */
