//
//  hitbox.h
//  shmup
//
//  Created by Raphaël Calabro on 27/02/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef hitbox_h
#define hitbox_h

#include "melstd.h"

#include "point.h"
#include "rectangle.h"
#include "inputstream.h"
#include "outputstream.h"

typedef struct melhitbox MELHitbox;
typedef struct melsprite MELSprite;

typedef struct {
    MELRectangle (* _Nonnull getFrame)(MELHitbox * _Nonnull self);
    void (* _Nullable deinit)(MELHitbox * _Nonnull self);
    void (* _Nullable save)(MELHitbox * _Nonnull self, MELOutputStream * _Nonnull outputStream);
} MELHitboxClass;

typedef struct melhitbox {
    const MELHitboxClass * _Nonnull class;
} MELHitbox;

MELHitbox * _Nullable MELHitboxLoad(MELInputStream * _Nonnull inputStream, MELSprite * _Nonnull sprite);
void MELHitboxDeinit(MELHitbox * _Nullable self);
void MELHitboxReaffect(MELHitbox * _Nonnull self, MELSprite * _Nonnull sprite);

MELRectangle MELHitboxGetFrame(MELHitbox * _Nonnull self);
MELBoolean MELHitboxCollidesWithPoint(MELHitbox * _Nonnull self, MELPoint point);
MELBoolean MELHitboxCollidesWithRectangle(MELHitbox * _Nonnull self, MELRectangle rectangle);
MELBoolean MELHitboxCollidesWithHitbox(MELHitbox * _Nonnull self, MELHitbox * _Nonnull other);

MELRectangle MELHitboxTopHalfRectangle(MELHitbox * _Nonnull self);
MELRectangle MELHitboxBottomQuarterRectangle(MELHitbox * _Nonnull self);

#endif /* hitbox_h */
