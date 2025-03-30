//
//  hash.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 19/06/2023.
//

#ifndef hash_h
#define hash_h

#include "melstd.h"

#define SHA256_BLOCK_SIZE 32

typedef uint8_t MELSHA256Hash[SHA256_BLOCK_SIZE];

typedef struct {
    uint8_t data[64];
    uint32_t dataLength;
    uint64_t bitCount;
    uint32_t state[8];
} MELSHA256Context;

void MELSHA256HashData(void * _Nonnull data, int length, MELSHA256Hash _Nonnull hash);

MELSHA256Context * _Nonnull MELSHA256Alloc(void);
void MELSHA256Update(MELSHA256Context * _Nonnull self, const void * _Nonnull data, int length);
void MELSHA256Final(MELSHA256Context * _Nonnull self, MELSHA256Hash _Nonnull hash);

MELBoolean MELSHA256AreEquals(MELSHA256Hash _Nonnull lhs, MELSHA256Hash _Nonnull rhs);

#endif /* hash_h */
