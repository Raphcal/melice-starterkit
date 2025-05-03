//
//  classes.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 28/11/2023.
//

#ifndef classes_h
#define classes_h

#include "common.h"

typedef struct melspriteclass MELSpriteClass;

typedef enum {
    SpriteClassNameDefault,
} SpriteClassName;

/**
 * Récupère la classe correspondante au nom <code>className</code> ou <code>NULL</code>.
 *
 * @param className Nom de la classe.
 * @returns La classe correspondante ou <code>NULL</code>.
 */
const MELSpriteClass * _Nullable MELSpriteClassForName(SpriteClassName className);

#endif /* classes_h */
