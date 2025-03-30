//
//  lcdspriteref.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 17/12/2023.
//

#ifndef lcdspriteref_h
#define lcdspriteref_h

#include "melstd.h"

#include "list.h"

typedef LCDSprite * _Nullable LCDSpriteRef;

MELListDefine(LCDSpriteRef);
MELListDefineIndexOf(LCDSpriteRef);

void LCDSpriteRefListDeallocReverse(LCDSpriteRefList * _Nonnull self);

void LCDSpriteDeinit(LCDSprite * _Nonnull sprite);
void LCDSpriteRefDeinit(LCDSpriteRef * _Nonnull self);

#endif /* lcdspriteref_h */
