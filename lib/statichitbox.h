//
//  statichitbox.h
//  shmup
//
//  Created by Raphaël Calabro on 07/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef statichitbox_h
#define statichitbox_h

#include "hitbox.h"

extern const MELHitboxClass MELStaticHitboxClass;

typedef struct {
    MELHitbox super;
    MELRectangle frame;
} MELStaticHitbox;

MELHitbox * _Nonnull MELStaticHitboxAlloc(MELRectangle frame);
MELHitbox * _Nonnull MELStaticHitboxLoad(MELInputStream * _Nonnull inputStream);

#endif /* statichitbox_h */
