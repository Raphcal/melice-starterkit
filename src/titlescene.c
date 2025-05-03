//
//  titlescene.c
//  relaxe
//
//  Created by Raphaël Calabro on 03/05/2025.
//

#include "titlescene.h"

TitleScene * _Nonnull TitleSceneAlloc(void) {
    TitleScene *self = new(TitleScene);
    *self = (TitleScene) {
        .super = {
        },
    };
    return self;
}
