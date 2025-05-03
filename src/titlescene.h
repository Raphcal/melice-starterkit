//
//  titlescene.h
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#ifndef titlescene_h
#define titlescene_h

#include "common.h"
#include "../lib/melice.h"

typedef struct {
    MELScene super;
} TitleScene;

TitleScene * _Nonnull TitleSceneAlloc(void);

#endif /* titlescene_h */
