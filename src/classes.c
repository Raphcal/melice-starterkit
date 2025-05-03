//
//  classes.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 28/11/2023.
//

#include "classes.h"

#include "../lib/melice.h"

const MELSpriteClass * _Nullable MELSpriteClassForName(SpriteClassName className) {
    switch (className) {
        case SpriteClassNameDefault:
            return &MELSpriteClassDefault;
        default:
            playdate->system->logToConsole("Unsupported class name: %d", className);
            return NULL;
    }
}
