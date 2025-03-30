//
//  primitives.c
//  melice
//
//  Created by Raphaël Calabro on 29/10/2021.
//

#include "primitives.h"

MELListImplement(float);
MELListImplement(MELByte);
MELListImplement(MELBoolean);
MELListImplement(MELRef);

void * _Nullable MELArrayCopy(const void * restrict _Nullable source, unsigned int size) {
    if (source == NULL || size == 0) {
        return NULL;
    }
    void *copy = playdate->system->realloc(NULL, size);
    memcpy(copy, source, size);
    return copy;
}

MELBoolean MELInt32ArrayIsFilledWithValue(const int32_t * _Nonnull self, unsigned int count, int32_t value) {
    for (unsigned int index = 0; index < count; index++) {
        if (self[index] != value) {
            return false;
        }
    }
    return true;
}

MELBoolean MELUInt32ArrayIsFilledWithValue(const uint32_t * _Nonnull self, unsigned int count, uint32_t value) {
    for (unsigned int index = 0; index < count; index++) {
        if (self[index] != value) {
            return false;
        }
    }
    return true;
}
