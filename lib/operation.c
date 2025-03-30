//
//  operation.c
//  shmup
//
//  Created by Raphaël Calabro on 08/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "operation.h"

#include <string.h>
#include "melmath.h"

MELOperation MELOperationMakeWithInputStream(MELInputStream * _Nonnull inputStream) {
    MELOperation self;
    self.code = MELInputStreamReadByteArray(inputStream, &self.count);
    return self;
}

MELOperation MELOperationMakeWithOperation(MELOperation other) {
    MELOperation self = other;
    self.code = MELArrayCopy(other.code, sizeof(uint8_t) * other.count);
    return self;
}

void MELOperationDeinit(MELOperation * _Nonnull operation) {
    playdate->system->realloc(operation->code, 0);
    operation->code = NULL;
    operation->count = 0;
}

typedef enum {
    MELByteCodeAdd = 0x2b,
    MELByteCodeSubstract = 0x2d,
    MELByteCodeMultiply = 0x2a,
    MELByteCodeDivide = 0x2f,
    MELByteCodePow = 0x5e,
    MELByteCodeNegative = 0x6e,
    MELByteCodeConstant = 0x43,
    MELByteCodeX = 0x78,
    MELByteCodePi = 0x70,
    MELByteCodeE = 0x65,
    MELByteCodeMinimum = 0x6d,
    MELByteCodeMaximum = 0x4d,
    MELByteCodeCosinus = 0x63,
    MELByteCodeSinus = 0x73,
    MELByteCodeSquareRoot = 0x53,
    MELByteCodeZoom = 0x7a,
    MELByteCodeSpriteVariable = 0x76,
    MELByteCodeSpriteDirection = 0x64,
    MELByteCodeSpriteHitboxTop = 0x68
} MELByteCode;

MELList(float) MELOperationExecute(MELOperation self, float x) {
    int32_t index = 0;
    floatList stack = floatListMakeWithInitialCapacity(self.count);

    float operand;

    while (index < self.count) {
        switch ((MELByteCode)self.code[index++]) {
            case MELByteCodeAdd:
                floatListPush(&stack, floatListPop(&stack) + floatListPop(&stack));
                break;

            case MELByteCodeSubstract:
                operand = floatListPop(&stack);
                floatListPush(&stack, floatListPop(&stack) - operand);
                break;

            case MELByteCodeMultiply:
                floatListPush(&stack, floatListPop(&stack) * floatListPop(&stack));
                break;

            case MELByteCodeDivide:
                operand = floatListPop(&stack);
                floatListPush(&stack, floatListPop(&stack) / operand);
                break;

            case MELByteCodePow:
                operand = floatListPop(&stack);
                floatListPush(&stack, powf(floatListPop(&stack), operand));
                break;

            case MELByteCodeNegative:
                floatListPush(&stack, -floatListPop(&stack));
                break;

            case MELByteCodeConstant:
                memcpy(&operand, self.code + index, sizeof(float));
                index += sizeof(float);
                floatListPush(&stack, operand);
                break;

            case MELByteCodeX:
                floatListPush(&stack, x);
                break;

            case MELByteCodePi:
                floatListPush(&stack, MEL_PI);
                break;

            case MELByteCodeE:
                floatListPush(&stack, MEL_PI);
                break;

            case MELByteCodeMinimum:
                floatListPush(&stack, MELFloatMin(floatListPop(&stack), floatListPop(&stack)));
                break;

            case MELByteCodeMaximum:
                floatListPush(&stack, MELFloatMax(floatListPop(&stack), floatListPop(&stack)));
                break;

            case MELByteCodeCosinus:
                floatListPush(&stack, cosf(floatListPop(&stack)));
                break;

            case MELByteCodeSinus:
                floatListPush(&stack, sinf(floatListPop(&stack)));
                break;

            case MELByteCodeSquareRoot:
                operand = floatListPop(&stack);
                if (operand >= 0) {
                    floatListPush(&stack, sqrtf(operand));
                } else {
                    floatListPush(&stack, -1);
                }
                break;

            case MELByteCodeZoom:
                break;

            default:
                break;
        }
    }

    return stack;
}
