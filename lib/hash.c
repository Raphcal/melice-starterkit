//
//  hash.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 19/06/2023.
//

#include "hash.h"

#include <string.h>

#define RIGHT_ROTATE(value, rotation) ((value >> rotation) | (value << (32 - rotation)))

static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256Transform(MELSHA256Context * _Nonnull self, const uint8_t * _Nonnull data) {
    uint32_t a, b, c, d, e, f, g, h, index, dataIndex, temp1, temp2, w[64];

    a = self->state[0];
    b = self->state[1];
    c = self->state[2];
    d = self->state[3];
    e = self->state[4];
    f = self->state[5];
    g = self->state[6];
    h = self->state[7];

    for (index = 0, dataIndex = 0; index < 16; index++, dataIndex += 4) {
        w[index] = (data[dataIndex] << 24) | (data[dataIndex + 1] << 16) | (data[dataIndex + 2] << 8) | (data[dataIndex + 3]);

        temp1 = h
            // S1
            + (RIGHT_ROTATE(e, 6) ^ RIGHT_ROTATE(e, 11) ^ RIGHT_ROTATE(e, 25))
            // ch
            + ((e & f) ^ (~e & g))
            + k[index] + w[index];
        temp2 = // S0
            (RIGHT_ROTATE(a, 2) ^ RIGHT_ROTATE(a, 13) ^ RIGHT_ROTATE(a, 22))
            // maj
            + ((a & b) ^ (a & c) ^ (b & c));

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    for (; index < 64; index++) {
        w[index] = w[index - 16]
        // s0
        + (RIGHT_ROTATE(w[index - 15], 7) ^ RIGHT_ROTATE(w[index - 15], 18) ^ (w[index - 15] >> 3))
        + w[index - 7]
        // s1
        + (RIGHT_ROTATE(w[index - 2], 17) ^ RIGHT_ROTATE(w[index - 2], 19) ^ (w[index - 2] >> 10));

        temp1 = h
            // S1
            + (RIGHT_ROTATE(e, 6) ^ RIGHT_ROTATE(e, 11) ^ RIGHT_ROTATE(e, 25))
            // ch
            + ((e & f) ^ (~e & g))
            + k[index] + w[index];
        temp2 = // S0
            (RIGHT_ROTATE(a, 2) ^ RIGHT_ROTATE(a, 13) ^ RIGHT_ROTATE(a, 22))
            // maj
            + ((a & b) ^ (a & c) ^ (b & c));
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    self->state[0] += a;
    self->state[1] += b;
    self->state[2] += c;
    self->state[3] += d;
    self->state[4] += e;
    self->state[5] += f;
    self->state[6] += g;
    self->state[7] += h;
}

static void sha256Init(MELSHA256Context * _Nonnull self) {
    self->dataLength = 0;
    self->bitCount = 0;
    self->state[0] = 0x6a09e667;
    self->state[1] = 0xbb67ae85;
    self->state[2] = 0x3c6ef372;
    self->state[3] = 0xa54ff53a;
    self->state[4] = 0x510e527f;
    self->state[5] = 0x9b05688c;
    self->state[6] = 0x1f83d9ab;
    self->state[7] = 0x5be0cd19;
}

MELSHA256Context * _Nonnull MELSHA256Alloc(void) {
    MELSHA256Context *self = playdate->system->realloc(NULL, sizeof(MELSHA256Context));
    sha256Init(self);
    return self;
}

static void sha256Update(MELSHA256Context * _Nonnull context, const uint8_t * _Nonnull data, uint32_t length) {
    MELSHA256Context self = *context;
    for (unsigned int i = 0; i < length; ++i) {
        self.data[self.dataLength] = data[i];
        self.dataLength++;
        if (self.dataLength == 64) {
            sha256Transform(&self, self.data);
            self.bitCount += 512;
            self.dataLength = 0;
        }
    }
    *context = self;
}

void MELSHA256Update(MELSHA256Context * _Nonnull self, const void * _Nonnull data, int length) {
    sha256Update(self, data, length);
}

void MELSHA256Final(MELSHA256Context * _Nonnull self, MELSHA256Hash _Nonnull hash) {
    unsigned int index = self->dataLength;

    // Pad the data with 0x80, followed by zeros.
    if (self->dataLength < 56) {
        self->data[index++] = 0x80;
        while (index < 56)
            self->data[index++] = 0x00;
    } else {
        self->data[index++] = 0x80;
        while (index < 64)
            self->data[index++] = 0x00;
        sha256Transform(self, self->data);
        memset(self->data, 0, 56);
    }

    // Append the bit length of the data.
    self->bitCount += self->dataLength * 8;
    self->data[63] = self->bitCount;
    self->data[62] = self->bitCount >> 8;
    self->data[61] = self->bitCount >> 16;
    self->data[60] = self->bitCount >> 24;
    self->data[59] = self->bitCount >> 32;
    self->data[58] = self->bitCount >> 40;
    self->data[57] = self->bitCount >> 48;
    self->data[56] = self->bitCount >> 56;
    sha256Transform(self, self->data);

    // Generate the hash.
    for (unsigned int i = 0; i < 4; ++i) {
        hash[i] = (self->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (self->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (self->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (self->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (self->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (self->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (self->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (self->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

void MELSHA256HashData(void * _Nonnull data, int length, MELSHA256Hash _Nonnull hash) {
    MELSHA256Context self;
    sha256Init(&self);
    sha256Update(&self, data, length);
    MELSHA256Final(&self, hash);
}

MELBoolean MELSHA256AreEquals(MELSHA256Hash lhs, MELSHA256Hash _Nonnull rhs) {
    for (unsigned int index = 0; index < SHA256_BLOCK_SIZE; index++) {
        if (lhs[index] != rhs[index]) {
            return false;
        }
    }
    return true;
}

int main(void) {
    MELSHA256Hash hash;

    char data[] = "Bonjour le monde";
    unsigned int len = (unsigned int) strlen(data);

    MELSHA256HashData(data, len, hash);

    printf("SHA-256 hash: ");
    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i)
        printf("%02x", hash[i]);
    printf("\n");

    return 0;
}
