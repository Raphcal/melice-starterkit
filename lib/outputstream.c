//
//  outputstream.c
//  melice
//
//  Created by Raphaël Calabro on 22/11/2021.
//

#include "outputstream.h"

#include "melstring.h"

#define MELOutputStreamBufferSize 4096

MELOutputStream MELOutputStreamOpen(const char * _Nonnull path) {
    return (MELOutputStream) {
        .file = playdate->file->open(path, kFileWrite),
        .buffer = playdate->system->realloc(NULL, sizeof(uint8_t) * MELOutputStreamBufferSize),
        .count = 0,
        .capacity = MELOutputStreamBufferSize,
        .sha256 = NULL,
    };
}

MELOutputStream MELOutputStreamInit(void) {
    return (MELOutputStream) {
        .file = NULL,
        .buffer = playdate->system->realloc(NULL, sizeof(uint8_t) * MELOutputStreamBufferSize),
        .count = 0,
        .capacity = MELOutputStreamBufferSize,
        .sha256 = NULL,
    };
}

int MELOutputStreamClose(MELOutputStream * _Nonnull self) {
    MELOutputStreamFlush(self);

    if (self == NULL) {
        playdate->system->error("Given outputstream is NULL");
        return 0;
    }
    self->count = 0;
    playdate->system->realloc(self->buffer, 0);
    self->buffer = NULL;

    if (self->file) {
        SDFile *file = self->file;
        self->file = NULL;
        return playdate->file->close(file);
    } else {
        return 0;
    }
}

void MELOutputStreamDeinit(MELOutputStream * _Nonnull self) {
    MELOutputStreamClose(self);
}

uint8_t * _Nonnull MELOutputStreamGetBytes(MELOutputStream self) {
    uint8_t * bytes = playdate->system->realloc(NULL, sizeof(uint8_t) * self.count);
    memcpy(bytes, self.buffer, sizeof(uint8_t) * self.count);
    return bytes;
}

void MELOutputStreamFlush(MELOutputStream * _Nonnull self) {
    if (self->file && self->count > 0) {
        const uint32_t size = sizeof(uint8_t) * self->count;
        playdate->file->write(self->file, self->buffer, size);
        if (self->sha256) {
            MELSHA256Update(self->sha256, self->buffer, size);
        }
        self->count = 0;
    } else if (!self->file && self->count >= self->capacity) {
        self->buffer = playdate->system->realloc(self->buffer, sizeof(uint8_t) * self->capacity * 2);
        self->capacity *= 2;
    }
}

void MELOutputStreamWriteUInt8(MELOutputStream * _Nonnull self, uint8_t byte) {
    self->buffer[self->count++] = byte;
    if (self->count == self->capacity) {
        MELOutputStreamFlush(self);
    }
}

void MELOutputStreamWriteInt8(MELOutputStream * _Nonnull self, int8_t byte) {
    MELOutputStreamWriteUInt8(self, byte & 0xFF);
}

void MELOutputStreamWrite(MELOutputStream * _Nonnull self, const void * _Nonnull source, unsigned int size) {
    unsigned int count;
    do {
        count = self->count + size > self->capacity ? self->capacity - self->count : size;
        memcpy(self->buffer + self->count, source, count);
        self->count += count;
        if (self->count == self->capacity) {
            MELOutputStreamFlush(self);
        }
        size -= count;
        source += count;
    } while (size > 0);
}

void MELOutputStreamWriteNumeric(MELOutputStream * _Nonnull self, int64_t value, unsigned int byteCount) {
    for(unsigned int index = 0; index < byteCount; index++) {
        MELOutputStreamWriteByte(self, value & 0xFF);
        value = value >> 8;
    }
}

void MELOutputStreamWriteZero(MELOutputStream * _Nonnull self, unsigned int size) {
    uint8_t zeroes[size];
    memset(zeroes, 0, sizeof(uint8_t) * size);
    MELOutputStreamWrite(self, zeroes, sizeof(uint8_t) * size);
}

void MELOutputStreamWriteBoolean(MELOutputStream * _Nonnull self, MELBoolean value) {
    MELOutputStreamWriteByte(self, value);
}

void MELOutputStreamWriteInt32(MELOutputStream * _Nonnull self, int32_t value) {
    MELOutputStreamWriteNumeric(self, value, sizeof(int32_t));
}

void MELOutputStreamWriteShort(MELOutputStream * _Nonnull self, int16_t value) {
    MELOutputStreamWriteNumeric(self, value, sizeof(int16_t));
}

void MELOutputStreamWriteUInt16(MELOutputStream * _Nonnull self, uint16_t value) {
    MELOutputStreamWriteNumeric(self, value, sizeof(uint16_t));
}

void MELOutputStreamWriteUInt32(MELOutputStream * _Nonnull self, uint32_t value) {
    MELOutputStreamWriteNumeric(self, value, sizeof(uint32_t));
}

void MELOutputStreamWriteFloat(MELOutputStream * _Nonnull self, float value) {
    union {
        float f;
        uint32_t u;
    } f = { .f = value };
    MELOutputStreamWriteNumeric(self, f.u, sizeof(float));
}

void MELOutputStreamWriteDouble(MELOutputStream * _Nonnull self, double value) {
    union {
        double d;
        uint64_t u;
    } d = { .d = value };
    MELOutputStreamWriteNumeric(self, d.u, sizeof(double));
}

void MELOutputStreamWriteByteArray(MELOutputStream * _Nonnull self, const uint8_t * _Nonnull value, int32_t count) {
    MELOutputStreamWriteInt(self, count);
    MELOutputStreamWrite(self, value, sizeof(uint8_t) * count);
}

void MELOutputStreamWriteInt8Array(MELOutputStream * _Nonnull self, const int8_t * _Nonnull value, int32_t count) {
    MELOutputStreamWriteInt(self, count);
    MELOutputStreamWrite(self, value, sizeof(int8_t) * count);
}

void MELOutputStreamWriteNullableByteArray(MELOutputStream * _Nonnull self, const uint8_t * _Nullable value, int32_t count) {
    MELOutputStreamWriteBoolean(self, value != NULL);
    if (value != NULL) {
        MELOutputStreamWriteByteArray(self, value, count);
    }
}

void MELOutputStreamWriteUInt8Array(MELOutputStream * _Nonnull self, const uint8_t * _Nonnull value, uint32_t count) {
    MELOutputStreamWriteUInt32(self, count);
    for (int32_t index = 0; index < count; index++) {
        MELOutputStreamWriteUInt8(self, value[index]);
    }
}

void MELOutputStreamWriteIntArray(MELOutputStream * _Nonnull self, const int32_t * _Nonnull value, int32_t count) {
    MELOutputStreamWriteInt(self, count);
    for (int32_t index = 0; index < count; index++) {
        MELOutputStreamWriteInt(self, value[index]);
    }
}

void MELOutputStreamWriteUInt32Array(MELOutputStream * _Nonnull self, const uint32_t * _Nonnull value, uint32_t count) {
    MELOutputStreamWriteUInt32(self, count);
    for (int32_t index = 0; index < count; index++) {
        MELOutputStreamWriteUInt32(self, value[index]);
    }
}

unsigned int str16len(uint16_t * _Nonnull array) {
    uint16_t *current;
    for (current = array; *current; current++);
    return (unsigned int) (current - array);
}

void MELOutputStreamWriteString(MELOutputStream * _Nonnull self, const char * _Nonnull value) {
    uint16_t *utf16String = MELUTF16StringMakeWithUTF8String(value);
    unsigned int count = str16len(utf16String);

    MELOutputStreamWriteInt(self, (int32_t) count);
    for (unsigned int index = 0; index < count; index++) {
        MELOutputStreamWriteUInt16(self, utf16String[index]);
    }
    playdate->system->realloc(utf16String, 0);
}

void MELOutputStreamWriteNullableString(MELOutputStream * _Nonnull self, const char * _Nullable value) {
    MELOutputStreamWriteBoolean(self, value != NULL);
    if (value != NULL) {
        MELOutputStreamWriteString(self, value);
    }
}

void MELOutputStreamWriteUTF8String(MELOutputStream * _Nonnull self, const char * _Nonnull value) {
    const unsigned int count = (unsigned int) strlen(value);

    MELOutputStreamWriteInt(self, (int32_t) count);
    MELOutputStreamWrite(self, value, sizeof(char) * count);
}

void MELOutputStreamWritePoint(MELOutputStream * _Nonnull self, MELPoint value) {
    MELOutputStreamWriteFloat(self, value.x);
    MELOutputStreamWriteFloat(self, value.y);
}

void MELOutputStreamWriteSize(MELOutputStream * _Nonnull self, MELSize value) {
    MELOutputStreamWriteFloat(self, value.width);
    MELOutputStreamWriteFloat(self, value.height);
}

void MELOutputStreamWriteRectangle(MELOutputStream * _Nonnull self, MELRectangle value) {
    MELOutputStreamWritePoint(self, value.origin);
    MELOutputStreamWriteSize(self, value.size);
}

void MELOutputStreamWriteIntPoint(MELOutputStream * _Nonnull self, MELIntPoint value) {
    MELOutputStreamWriteInt(self, value.x);
    MELOutputStreamWriteInt(self, value.y);
}

void MELOutputStreamWriteIntSize(MELOutputStream * _Nonnull self, MELIntSize value) {
    MELOutputStreamWriteInt(self, value.width);
    MELOutputStreamWriteInt(self, value.height);
}

void MELOutputStreamWriteIntRectangle(MELOutputStream * _Nonnull self, MELIntRectangle value) {
    MELOutputStreamWriteIntPoint(self, value.origin);
    MELOutputStreamWriteIntSize(self, value.size);
}

void MELOutputStreamWriteUUID(MELOutputStream * _Nonnull self, MELUUID uuid) {
    MELOutputStreamWrite(self, uuid.data, sizeof(uint8_t) * kUUIDByteCount);
}

void MELOutputStreamWritePDScore(MELOutputStream * _Nonnull self, PDScore score) {
    MELOutputStreamWriteNullableString(self, score.player);
    MELOutputStreamWriteUInt32(self, score.value);
    MELOutputStreamWriteUInt32(self, score.rank);
}

void MELOutputStreamWritePDScoreArray(MELOutputStream * _Nonnull self, const PDScore * _Nonnull scores, uint32_t count) {
    MELOutputStreamWriteUInt32(self, count);
    for (int32_t index = 0; index < count; index++) {
        MELOutputStreamWritePDScore(self, scores[index]);
    }
}

#pragma mark - SHA256

void MELOutputStreamStartHash(MELOutputStream * _Nonnull self, const char * _Nonnull salt, const unsigned int saltLength) {
    MELOutputStreamFlush(self);
    self->hashStart = playdate->file->tell(self->file);

    MELSHA256Hash hash = {};
    playdate->file->write(self->file, hash, sizeof(MELSHA256Hash));

    self->sha256 = MELSHA256Alloc();
    MELSHA256Update(self->sha256, salt, saltLength);
}

void MELOutputStreamEndHash(MELOutputStream * _Nonnull self) {
    MELOutputStreamFlush(self);
    MELSHA256Context *sha256 = self->sha256;
    self->sha256 = NULL;
    MELSHA256Hash hash;
    MELSHA256Final(sha256, hash);
    playdate->system->realloc(sha256, 0);

    playdate->file->seek(self->file, self->hashStart, SEEK_SET);
    playdate->file->write(self->file, hash, sizeof(MELSHA256Hash));

    playdate->file->seek(self->file, 0, SEEK_END);
}
