//
//  statichitbox.c
//  shmup
//
//  Created by Raphaël Calabro on 07/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "statichitbox.h"

#include "hitboxtype.h"

static MELRectangle getFrame(MELStaticHitbox * _Nonnull self) { 
    return self->frame;
}

static void save(MELStaticHitbox * _Nonnull self, MELOutputStream * _Nonnull outputStream) {
    MELOutputStreamWriteByte(outputStream, MELHitboxTypeStaticHitbox);
    MELOutputStreamWriteRectangle(outputStream, self->frame);
}

const MELHitboxClass MELStaticHitboxClass = {
    .getFrame = (MELRectangle(*)(MELHitbox *)) getFrame,
    .save = (void(*)(MELHitbox *, MELOutputStream *)) save,
};

MELHitbox * _Nonnull MELStaticHitboxAlloc(MELRectangle frame) {
    MELStaticHitbox *self = malloc(sizeof(MELStaticHitbox));
    *self = (MELStaticHitbox) {
        .super = (MELHitbox) {
            .class = &MELStaticHitboxClass
        },
        .frame = frame
    };
    return (MELHitbox *)self;
}

MELHitbox * _Nonnull MELStaticHitboxLoad(MELInputStream * _Nonnull inputStream) {
    return MELStaticHitboxAlloc(MELInputStreamReadRectangle(inputStream));
}
