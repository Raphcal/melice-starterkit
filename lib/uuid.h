//
//  uuid.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 18/11/2024.
//

#ifndef uuid_h
#define uuid_h

#include "melstd.h"

#define kUUIDByteCount 16

typedef struct {
    uint8_t data[kUUIDByteCount];
} MELUUID;

MELUUID MELUUIDMakeV4(void);
MELUUID MELUUIDMakeV7(void);

void MELUUIDPrint(MELUUID uuid);

MELBoolean MELUUIDIsEmpty(MELUUID uuid);
MELBoolean MELUUIDEquals(MELUUID lhs, MELUUID rhs);

uint32_t MELUUIDGetHashcode(MELUUID uuid);

#endif /* uuid_h */
