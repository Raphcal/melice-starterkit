//
//  animationframe.c
//  shmup
//
//  Created by Raphaël Calabro on 28/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "animationframe.h"

MELAnimationFrame MELAnimationFrameMake(unsigned int atlasIndex, MELRectangle hitbox) {
    return (MELAnimationFrame) {
        atlasIndex,
        hitbox
    };
}

MELAnimationFrame MELAnimationFrameMakeWithInputStream(MELInputStream * _Nonnull inputStream) {
    MELAnimationFrame self;
    self.atlasIndex = MELInputStreamReadInt(inputStream);
    const MELBoolean hasHitbox = MELInputStreamReadBoolean(inputStream);
    MELRectangle hitbox;
    if (hasHitbox) {
        hitbox.origin.x = MELInputStreamReadInt(inputStream);
        hitbox.origin.y = MELInputStreamReadInt(inputStream);
        hitbox.size.width = MELInputStreamReadInt(inputStream);
        hitbox.size.height = MELInputStreamReadInt(inputStream);
    } else {
        hitbox = (MELRectangle) {};
    }
    self.hitbox = hitbox;
    return self;
}
