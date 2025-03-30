//
//  platform.h
//  Roll
//
//  Created by Raphaël Calabro on 12/09/2022.
//

#ifndef platform_h
#define platform_h

#include "sprite.h"

typedef struct {
    MELSprite super;
    MELSprite * _Nullable landed;
    // Pour calculer le delta et déplacer le joueur en même temps que la plateforme.
    MELPoint oldOrigin;
} MELPlatform;

#endif /* platform_h */
