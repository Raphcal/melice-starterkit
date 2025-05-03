//
//  inputstream.c
//  shmup
//
//  Created by Raphaël Calabro on 25/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "inputstream.h"

#include <string.h>

#include "melstring.h"
#include "primitives.h"

#define MELInputStreamBufferSize 4096

MELBoolean MELFileExists(const char * _Nonnull path) {
    FileStat stat;
    return playdate->file->stat(path, &stat) == 0;
}

MELInputStream MELInputStreamOpen(const char * _Nonnull path, FileOptions mode) {
    return (MELInputStream) {
        .file = playdate->file->open(path, mode),
        .buffer = playdate->system->realloc(NULL, sizeof(uint8_t) * MELInputStreamBufferSize),
        .size = 0,
        .cursor = 0,
        .sha256 = NULL,
    };
}

int MELInputStreamClose(MELInputStream * _Nonnull self) {
    if (self == NULL) {
        playdate->system->error("Given inputstream is NULL");
        return 0;
    }
    self->cursor = 0;
    self->size = 0;
    if (self->buffer) {
        playdate->system->realloc(self->buffer, 0);
        self->buffer = NULL;
    }
    if (self->file) {
        SDFile *file = self->file;
        self->file = NULL;
        return playdate->file->close(file);
    } else {
        return 0;
    }
}

MELInputStream MELInputStreamInitWithBytes(const void * _Nonnull bytes, unsigned int count) {
    return (MELInputStream) {
        .file = NULL,
        .buffer = MELArrayCopy(bytes, count),
        .size = count,
        .cursor = 0,
        .sha256 = NULL,
    };
}

void MELInputStreamDeinit(MELInputStream * _Nonnull self) {
    MELInputStreamClose(self);
}

int MELInputStreamSeek(MELInputStream * _Nonnull self, int offset, MELInputStreamSeekFrom from) {
    if (self->file) {
        self->cursor = 0;
        self->size = 0;
        return playdate->file->seek(self->file, offset, from);
    } else {
        int destination;
        switch (from) {
            case MELInputStreamSeekFromStart:
                destination = offset;
                break;
            case MELInputStreamSeekFromCurrent:
                destination = self->cursor + offset;
                break;
            case MELInputStreamSeekFromEnd:
                destination = self->size + offset;
                break;
            default:
                // TODO: Return EINVAL
                return 1;
        }
        if (destination < 0 || destination >= self->size) {
            // TODO: Return EINVAL
            return 1;
        }
        self->cursor = destination;
    }
    return 0;
}

int MELInputStreamRemaining(MELInputStream * _Nonnull self) {
    return self->size - self->cursor;
}

void MELInputStreamCompact(MELInputStream * _Nonnull self) {
    const unsigned int cursor = self->cursor;
    if (cursor == 0) {
        return;
    }
    const unsigned int remaining = self->size - cursor;
    uint8_t *buffer = self->buffer;
    memmove(buffer, buffer + cursor, remaining);

    self->cursor = 0;
    self->size = remaining;
}

void MELInputStreamFillBuffer(MELInputStream * _Nonnull self) {
    if (self->file == NULL) {
        playdate->system->error("Unable to fill buffer: file is NULL");
        return;
    }
    const int size = self->size;
    int read = playdate->file->read(self->file, self->buffer + size, sizeof(uint8_t) * (MELInputStreamBufferSize - size));
    if (read < 1) {
        if (self->exceptionHandler) {
            playdate->system->logToConsole("Unable to fill buffer: EOF reached");
            longjmp(*self->exceptionHandler, 1);
        }
        playdate->system->error("Unable to fill buffer: EOF reached");
        return;
    }
    self->size = size + read;

    if (self->sha256) {
        MELSHA256Update(self->sha256, self->buffer + size, read);
    }
}

void MELInputStreamRead(MELInputStream * _Nonnull self, void * _Nonnull destination, unsigned int size) {
    if (size == 0) {
        return;
    }
    const int capacity = self->size > MELInputStreamBufferSize ? self->size : MELInputStreamBufferSize;
    if (size > capacity && self->file != NULL) {
        const unsigned int readCountFromBuffer = self->size - self->cursor;
        memcpy(destination, self->buffer + self->cursor, readCountFromBuffer);
        playdate->file->read(self->file, destination + readCountFromBuffer, (size - readCountFromBuffer) * sizeof(uint8_t));
        self->size = 0;
        self->cursor = 0;
        return;
    }
    if (MELInputStreamRemaining(self) < size) {
        MELInputStreamCompact(self);
        MELInputStreamFillBuffer(self);
        if (MELInputStreamRemaining(self) < size) {
            playdate->system->error("Unable to read %d bytes from given inputstream, only %d remaining.", size, MELInputStreamRemaining(self));
            memset(destination, 0, size);
            return;
        }
    }
    const unsigned int cursor = self->cursor;
    memcpy(destination, self->buffer + cursor, size);
    self->cursor = cursor + size;
}

int MELInputStreamReadUInt8(MELInputStream * _Nonnull self) {
    if (self->cursor == self->size) {
        if (self->file == NULL) {
            return -1;
        }
        self->size = 0;
        self->cursor = 0;
        MELInputStreamFillBuffer(self);
    }
    return self->buffer[self->cursor++];
}

int8_t MELInputStreamReadInt8(MELInputStream * _Nonnull self) {
    int8_t value;
    MELInputStreamRead(self, &value, sizeof(int8_t));
    return value;
}

void MELInputStreamSkipBytes(MELInputStream * _Nonnull self, unsigned int size) {
    if (size == 0) {
        return;
    }
    if (MELInputStreamRemaining(self) < size) {
        size -= self->size - self->cursor;
        self->cursor = 0;
        self->size = 0;
        MELInputStreamFillBuffer(self);
        if (MELInputStreamRemaining(self) < size) {
            playdate->system->error("Unable to skip %d bytes of given inputstream, only %d remaining.", size, MELInputStreamRemaining(self));
            return;
        }
    }
    self->cursor += size;
}

MELBoolean MELInputStreamReadBoolean(MELInputStream * _Nonnull self) {
    uint8_t value;
    MELInputStreamRead(self, &value, sizeof(uint8_t));
    return (MELBoolean)value;
}

int32_t MELInputStreamReadInt(MELInputStream * _Nonnull self) {
    int32_t value;
    MELInputStreamRead(self, &value, sizeof(int32_t));
    return value;
}

uint32_t MELInputStreamReadUInt32(MELInputStream * _Nonnull self) {
    uint32_t value;
    MELInputStreamRead(self, &value, sizeof(uint32_t));
    return value;
}

int16_t MELInputStreamReadShort(MELInputStream * _Nonnull self) {
    int16_t value;
    MELInputStreamRead(self, &value, sizeof(int16_t));
    return value;
}

uint16_t MELInputStreamReadUInt16(MELInputStream * _Nonnull self) {
    uint16_t value;
    MELInputStreamRead(self, &value, sizeof(uint16_t));
    return value;
}

float MELInputStreamReadFloat(MELInputStream * _Nonnull self) {
    float value;
    MELInputStreamRead(self, &value, sizeof(float));
    return value;
}

double MELInputStreamReadDouble(MELInputStream * _Nonnull self) {
    double value;
    MELInputStreamRead(self, &value, sizeof(double));
    return value;
}

uint8_t * _Nonnull MELInputStreamReadByteArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count) {
    *count = MELInputStreamReadInt(self);
    unsigned int size = sizeof(uint8_t) * *count;

    uint8_t *array = playdate->system->realloc(NULL, size);
    MELInputStreamRead(self, array, size);
    return array;
}

uint8_t * _Nullable MELInputStreamReadNullableByteArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count) {
    if (MELInputStreamReadBoolean(self)) {
        return MELInputStreamReadByteArray(self, count);
    } else {
        *count = 0;
        return NULL;
    }
}

int8_t * _Nonnull MELInputStreamReadInt8Array(MELInputStream * _Nonnull self, int32_t * _Nullable outCount) {
    const int count = MELInputStreamReadInt(self);
    if (outCount) {
        *outCount = count;
    }
    const unsigned int size = sizeof(int8_t) * count;

    int8_t *array = playdate->system->realloc(NULL, size);
    MELInputStreamRead(self, array, size);
    return array;
}

uint8_t * _Nonnull MELInputStreamReadUInt8Array(MELInputStream * _Nonnull self, uint32_t * _Nonnull count) {
    *count = MELInputStreamReadUInt32(self);
    unsigned int size = sizeof(uint8_t) * *count;
    
    uint8_t *array = playdate->system->realloc(NULL, size);
    MELInputStreamRead(self, array, size);
    return array;
}

int32_t * _Nonnull MELInputStreamReadIntArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count) {
    *count = MELInputStreamReadInt(self);
    unsigned int size = sizeof(int32_t) * *count;
    
    int32_t *array = playdate->system->realloc(NULL, size);
    MELInputStreamRead(self, array, size);
    return array;
}

uint32_t * _Nonnull MELInputStreamReadUInt32Array(MELInputStream * _Nonnull self, uint32_t * _Nonnull count) {
    *count = MELInputStreamReadUInt32(self);
    unsigned int size = sizeof(uint32_t) * *count;
    
    uint32_t *array = playdate->system->realloc(NULL, size);
    MELInputStreamRead(self, array, size);
    return array;
}

char * _Nonnull MELInputStreamReadString(MELInputStream * _Nonnull self) {
    int32_t count = MELInputStreamReadInt(self);

    uint16_t *string = playdate->system->realloc(NULL, sizeof(uint16_t) * (count + 1));
    MELInputStreamRead(self, string, sizeof(uint16_t) * count);
    string[count] = '\0';

    char *utf8String = MELUTF8StringMakeWithUTF16String(string);
    playdate->system->realloc(string, 0);

    return utf8String;
}

char * _Nullable MELInputStreamReadNullableString(MELInputStream * _Nonnull self) {
    if (MELInputStreamReadBoolean(self)) {
        return MELInputStreamReadString(self);
    } else {
        return NULL;
    }
}

char * _Nonnull MELInputStreamReadUTF8String(MELInputStream * _Nonnull self, unsigned int * _Nullable length) {
    int32_t count = MELInputStreamReadInt(self);
    if (length) {
        *length = count;
    }

    char *string = playdate->system->realloc(NULL, sizeof(char) * (count + 1));
    MELInputStreamRead(self, string, sizeof(char) * count);
    string[count] = '\0';

    return string;
}

LCDColor MELInputStreamReadColor(MELInputStream * _Nonnull self) {
    // TODO: Lire les couleurs.
    return kColorWhite;
}

MELPoint MELInputStreamReadPoint(MELInputStream * _Nonnull self) {
    const float x = MELInputStreamReadFloat(self);
    const float y = MELInputStreamReadFloat(self);
    return (MELPoint) {
        .x = x,
        .y = y
    };
}

MELSize MELInputStreamReadSize(MELInputStream * _Nonnull self) {
    const float width = MELInputStreamReadFloat(self);
    const float height = MELInputStreamReadFloat(self);
    return (MELSize) {
        .width = width,
        .height = height
    };
}

MELRectangle MELInputStreamReadRectangle(MELInputStream * _Nonnull self) {
    const MELPoint origin = MELInputStreamReadPoint(self);
    const MELSize size = MELInputStreamReadSize(self);
    return (MELRectangle) {
        .origin = origin,
        .size = size
    };
}

MELIntPoint MELInputStreamReadIntPoint(MELInputStream * _Nonnull self) {
    const int32_t x = MELInputStreamReadInt(self);
    const int32_t y = MELInputStreamReadInt(self);
    return (MELIntPoint) {
        .x = x,
        .y = y
    };
}

MELIntSize MELInputStreamReadIntSize(MELInputStream * _Nonnull self) {
    const int32_t width = MELInputStreamReadInt(self);
    const int32_t height = MELInputStreamReadInt(self);
    return (MELIntSize) {
        .width = width,
        .height = height
    };
}

MELIntRectangle MELInputStreamReadIntRectangle(MELInputStream * _Nonnull self) {
    const MELIntPoint origin = MELInputStreamReadIntPoint(self);
    const MELIntSize size = MELInputStreamReadIntSize(self);
    return (MELIntRectangle) {
        .origin = origin,
        .size = size
    };
}

PDScore MELInputStreamReadPDScore(MELInputStream * _Nonnull self) {
    PDScore score = (PDScore) {};
    score.player = MELInputStreamReadNullableString(self);
    score.value = MELInputStreamReadUInt32(self);
    score.rank = MELInputStreamReadUInt32(self);
    return score;
}

PDScore * _Nonnull MELInputStreamReadPDScoreArray(MELInputStream * _Nonnull self, uint32_t * _Nullable count) {
    PDScore *scores = NULL;
    uint32_t scoreCount = MELInputStreamReadUInt32(self);
    if (count != NULL) {
        *count = scoreCount;
    }
    if (scoreCount) {
        scores = playdate->system->realloc(NULL, sizeof(PDScore) * scoreCount);
        for (int32_t index = 0; index < scoreCount; index++) {
            scores[index] = MELInputStreamReadPDScore(self);
        }
    }
    return scores;
}


MELUUID MELInputStreamReadUUID(MELInputStream * _Nonnull self) {
    MELUUID uuid = (MELUUID) {};
    MELInputStreamRead(self, &uuid.data, sizeof(uint8_t) * kUUIDByteCount);
    return uuid;
}

#pragma mark - SHA256

void MELInputStreamStartHash(MELInputStream * _Nonnull self, const char * _Nonnull salt, const unsigned int saltLength) {
    const int hashStart = playdate->file->tell(self->file) - self->size + self->cursor;
    // Sauvegarde la position du hash pour comparer à la fin de la lecture.
    self->hashStart = hashStart;

    // Saute le hash pour ne pas le prendre en compte dans le calcul.
    MELInputStreamSeek(self, hashStart + sizeof(MELSHA256Hash), MELInputStreamSeekFromStart);

    self->sha256 = MELSHA256Alloc();
    MELSHA256Update(self->sha256, salt, saltLength);
}

MELBoolean MELInputStreamEndAndValidateHash(MELInputStream * _Nonnull self) {
    const int location = playdate->file->tell(self->file);

    // Calcul du hash SHA256
    MELSHA256Hash hash;
    MELSHA256Context *sha256 = self->sha256;
    MELSHA256Final(sha256, hash);
    playdate->system->realloc(sha256, 0);
    self->sha256 = NULL;

    // Lecture du hash sauvegardé.
    MELSHA256Hash savedHash;
    playdate->file->seek(self->file, self->hashStart, SEEK_SET);
    playdate->file->read(self->file, savedHash, sizeof(MELSHA256Hash));

    // Retour à la position actuelle.
    playdate->file->seek(self->file, location, SEEK_SET);

    return MELSHA256AreEquals(hash, savedHash);
}

