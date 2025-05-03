//
//  fadetoblackscene.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 04/08/2024.
//

#ifndef fadetoblackscene_h
#define fadetoblackscene_h

#include "melstd.h"

#include "scene.h"

MELScene * _Nonnull MELFadeToBlackSceneAlloc(MELScene * _Nonnull oldScene, MELScene * _Nonnull nextScene);

#endif /* fadetoblackscene_h */
