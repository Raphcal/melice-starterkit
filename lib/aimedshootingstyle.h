//
//  aimedshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 10/02/2023.
//

#ifndef aimedshootingstyle_h
#define aimedshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

typedef struct {
    MELShootingStyleDefinition super;
    LCDSprite * _Nonnull (* _Nullable getTarget)(void * _Nullable userdata);
} AimedShootingStyleDefinition;

void AimedShootingStyleInit(MELShootingStyle * _Nonnull self, const AimedShootingStyleDefinition * _Nonnull definition);

#endif /* aimedshootingstyle_h */
