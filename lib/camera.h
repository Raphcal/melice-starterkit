//
//  camera.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef camera_h
#define camera_h

#include "melstd.h"

#include "rectangle.h"

typedef struct {
    /**
     * Rectangle visible à l'écran. L'origine est en haut à gauche.
     */
    MELRectangle frame;
} MELCamera;

extern MELCamera camera;

/**
 * Effet de tremblement de terre.
 *
 * @param duration Durée en seconde du tremblement.
 * @param intensity Intensité du tremblement.
 */
LCDSprite * _Nonnull MELCameraShakeConstructor(float duration, float intensity);

#endif /* camera_h */
