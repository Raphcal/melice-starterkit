//
//  operation.h
//  shmup
//
//  Created by Raphaël Calabro on 08/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef operation_h
#define operation_h

#include "melstd.h"

#include "list.h"
#include "inputstream.h"
#include "primitives.h"

typedef struct {
    uint8_t * _Nullable code;
    int32_t count;
} MELOperation;

MELOperation MELOperationMakeWithInputStream(MELInputStream * _Nonnull inputStream);
MELOperation MELOperationMakeWithOperation(MELOperation other);
void MELOperationDeinit(MELOperation * _Nonnull operation);

MELList(float) MELOperationExecute(MELOperation self, float x);

#endif /* operation_h */
