//
//  melstring.c
//  melice
//
//  Created by Raphaël Calabro on 09/11/2021.
//

#include "melstring.h"

#include <string.h>
#include "melmath.h"

MELListImplement(MELChar);
MELListImplement(MELChar16);
MELListImplement(MELCodePoint);

const char kEmptyString[] = "";

MELBoolean MELStringEquals(const char * _Nonnull lhs, const char * _Nonnull rhs) {
    return !strcmp(lhs, rhs);
}
MELBoolean MELStringStartsWith(const char * _Nonnull lhs, const char * _Nonnull rhs) {
    const unsigned int lhsLength = (unsigned int) strlen(lhs);
    const unsigned int rhsLength = (unsigned int) strlen(rhs);
    if (lhsLength < rhsLength) {
        return false;
    }
    return !strncmp(lhs, rhs, rhsLength);
}
MELBoolean MELStringEndsWith(const char * _Nonnull lhs, const char * _Nonnull rhs) {
    const unsigned int lhsLength = (unsigned int) strlen(lhs);
    const unsigned int rhsLength = (unsigned int) strlen(rhs);
    if (lhsLength < rhsLength) {
        return false;
    }
    return !strcmp(lhs + (lhsLength - rhsLength), rhs);
}

uint64_t MELStringHash(const char * _Nonnull key) {
    uint64_t hash = 0;
    for (uint64_t index = 0; key[index]; index++) {
        hash = hash * index + key[index];
    }
    return hash;
}

char * _Nonnull MELStringCopy(const char * _Nullable source) {
    const unsigned int length = (unsigned int) strlen(source);
    char *copy = playdate->system->realloc(NULL, (length + 1) * sizeof(char));
    memcpy(copy, source, length * sizeof(char));
    copy[length] = '\0';
    return copy;
}

char * _Nonnull MELStringConcat(const char * _Nullable lhs, const char * _Nullable rhs) {
    const unsigned int lhsLength = (unsigned int) strlen(lhs);
    const unsigned int rhsLength = (unsigned int) strlen(rhs);
    char *result = playdate->system->realloc(NULL, (lhsLength + rhsLength + 1) * sizeof(char));
    memcpy(result, lhs, lhsLength * sizeof(char));
    memcpy(result + lhsLength, rhs, rhsLength * sizeof(char));
    result[lhsLength + rhsLength] = '\0';
    return result;
}

static MELBoolean isUTF8Wagon(const char * _Nonnull source, unsigned int index) {
    return ((source[index] & 0xFF) >> 6) == 2;
}

static MELBoolean isTrailedByCountUTF8Wagon(const char * _Nonnull source, unsigned int from, unsigned int count) {
    for (unsigned int index = 0; index < count; index++) {
        if (!isUTF8Wagon(source, from + index + 1)) {
            return false;
        }
    }
    return true;
}

MELCodePointList MELCodePointListMakeWithUTF8String(const char * _Nullable source) {
    MELCodePointList codePoints = MELCodePointListEmpty;
    MELCodePointListMakeWithUTF8StringAndBuffer(source, &codePoints);
    return codePoints;
}

void MELCodePointListMakeWithUTF8StringAndBuffer(const char * _Nullable source, MELCodePointList * _Nonnull codePoints) {
    codePoints->count = 0;
    if (source == NULL) {
        return;
    }
    MELCodePoint codePoint;
    uint32_t entry;
    for (unsigned int index = 0; source[index] != 0; index++) {
        entry = source[index] & 0xFF;
        if (entry <= 127) {
            // ASCII
            codePoint = entry;
        } else if (entry >> 5 == 6 && isUTF8Wagon(source, index + 1)) {
            // 2 bytes
            uint32_t byte1 = entry & 31;
            uint32_t byte2 = source[index + 1] & 63;
            codePoint = (byte1 << 6) | byte2;
            index++;
        } else if (entry >> 4 == 14 && isTrailedByCountUTF8Wagon(source, index, 2)) {
            // 3 bytes
            uint32_t byte1 = entry & 15;
            uint32_t byte2 = source[index + 1] & 63;
            uint32_t byte3 = source[index + 2] & 63;
            codePoint = (byte1 << 12) | (byte2 << 6) | byte3;
            index += 2;
        } else if (entry >> 3 == 30 && isTrailedByCountUTF8Wagon(source, index, 3)) {
            // 4 bytes
            uint32_t byte1 = entry & 7;
            uint32_t byte2 = source[index + 1] & 63;
            uint32_t byte3 = source[index + 2] & 63;
            uint32_t byte4 = source[index + 3] & 63;
            codePoint = (byte1 << 18) | (byte2 << 12) | (byte3 << 6) | byte4;
            index += 3;
        } else {
            // Encoding error
            codePoint = 0xFFFD;
        }
        MELCodePointListPush(codePoints, codePoint);
    }
}

MELCodePointList MELCodePointListMakeWithUTF16String(const uint16_t * _Nullable source) {
    if (source == NULL) {
        return MELCodePointListEmpty;
    }
    MELCodePointList codePoints = MELCodePointListEmpty;
    MELCodePoint codePoint;
    uint16_t entry, nextEntry;
    for (unsigned int index = 0; source[index] > 0; index++) {
        entry = source[index];
        nextEntry = source[index + 1];
        if (entry <= 0xD7FF || entry >= 0xE000) {
            // Basic Multilingual Plane
            codePoint = entry;
        } else if ((entry >= 0xD800 && entry <= 0xDBFF) && (nextEntry >= 0xDC00 && nextEntry <= 0xDFFF)) {
            // High surrogate - low surrogate
            codePoint = (entry & 0x3FF) << 10 | (nextEntry & 0x3FF) | 0x10000;
            index++;
        } else if ((entry >= 0xDC00 && entry <= 0xDFFF) && (nextEntry >= 0xD800 && nextEntry <= 0xDBFF)) {
            // Low surrogate - high surrogate
            codePoint = (nextEntry & 0x3FF) << 10 | (entry & 0x3FF) | 0x10000;
            index++;
        } else {
            // Encoding error
            codePoint = 0xFFFD;
        }
        MELCodePointListPush(&codePoints, codePoint);
    }
    return codePoints;
}

char * _Nullable MELUTF8StringMakeWithCodePoints(MELCodePointList codePoints) {
    MELCharList characters = MELCharListEmpty;
    for (unsigned int index = 0; index < codePoints.count; index++) {
        MELCodePoint codePoint = codePoints.memory[index];
        if (codePoint <= 0x007F) {
            MELCharListPush(&characters, codePoint);
        } else if (codePoint <= 0x07FF) {
            MELCharListPush(&characters, (codePoint >> 6) | 192);
            MELCharListPush(&characters, (codePoint & 63) | 128);
        } else if (codePoint <= 0xFFFF) {
            MELCharListPush(&characters, (codePoint >> 12) | 224);
            MELCharListPush(&characters, ((codePoint >> 6) & 63) | 128);
            MELCharListPush(&characters, (codePoint & 63) | 128);
        } else if (codePoint <= 0x10FFFF) {
            MELCharListPush(&characters, (codePoint >> 18) | 240);
            MELCharListPush(&characters, ((codePoint >> 12) & 63) | 128);
            MELCharListPush(&characters, ((codePoint >> 6) & 63) | 128);
            MELCharListPush(&characters, (codePoint & 63) | 128);
        } else {
            // Encoding error
            MELCharListPush(&characters, 0x3F);
        }
    }
    MELCharListPush(&characters, 0);
    return playdate->system->realloc(characters.memory, characters.count * sizeof(char));
}

char * _Nullable MELUTF8StringMakeWithUTF16String(const uint16_t * _Nullable source) {
    if (source == NULL) {
        return NULL;
    }
    MELCodePointList codePoints = MELCodePointListMakeWithUTF16String(source);
    char *utf8String = MELUTF8StringMakeWithCodePoints(codePoints);
    MELCodePointListDeinit(&codePoints);
    return utf8String;
}

uint32_t MELUTF8StringCodePointCount(const MELChar * _Nullable source, int length) {
    if (source == NULL) {
        return 0;
    }
    uint32_t count = 0;
    for (uint32_t index = 0; index < length && source[index] != '\0'; index++) {
        const uint32_t entry = source[index] & 0xFF;
        if (entry <= 127) {
            // ASCII
            count++;
        } else if (entry >> 5 == 6 && isUTF8Wagon(source, index + 1)) {
            // 2 bytes
            count++;
            index++;
        } else if (entry >> 4 == 14 && isTrailedByCountUTF8Wagon(source, index, 2)) {
            // 3 bytes
            count++;
            index += 2;
        } else if (entry >> 3 == 30 && isTrailedByCountUTF8Wagon(source, index, 3)) {
            // 4 bytes
            count++;
            index += 3;
        } else {
            // Encoding error
            playdate->system->logToConsole("Unable to count UTF-8 characters: encoding error, invalid UTF-8 value.");
        }
    }
    return count;
}

int MELUTF8StringCharacterSize(const char * _Nullable source, int index) {
    const uint32_t entry = source[index] & 0xFF;
    if (entry <= 127) {
        // ASCII
        return 1;
    } else if (entry >> 5 == 6 && isUTF8Wagon(source, index + 1)) {
        // 2 bytes
        return 2;
    } else if (entry >> 4 == 14 && isTrailedByCountUTF8Wagon(source, index, 2)) {
        // 3 bytes
        return 3;
    } else if (entry >> 3 == 30 && isTrailedByCountUTF8Wagon(source, index, 3)) {
        // 4 bytes
        return 4;
    } else {
        // Encoding error
        playdate->system->logToConsole("Unable to count UTF-8 characters: encoding error, invalid UTF-8 value.");
        return 1;
    }
}

uint16_t * _Nullable MELUTF16StringMakeWithCodePoints(MELCodePointList codePoints) {
    MELChar16List characters = MELChar16ListEmpty;
    for (unsigned int index = 0; index < codePoints.count; index++) {
        MELCodePoint codePoint = codePoints.memory[index];
        if (codePoint <= 0xD7FF || (codePoint >= 0xE000 && codePoint <= 0xFFFF)) {
            // Basic Multilingual Plane
            MELChar16ListPush(&characters, codePoint);
        } else if (codePoint >= 0x010000 && codePoint <= 0x10FFFF) {
            // Supplementary Planes
            MELCodePoint valueToEncode = codePoint - 0x010000;
            // High surrogate
            MELChar16ListPush(&characters, (valueToEncode >> 10) | 0xD800);
            // Low surrogate
            MELChar16ListPush(&characters, (valueToEncode & 0x3FF) | 0xDC00);
        } else {
            // Encoding error
            MELChar16ListPush(&characters, 0xFFFD);
        }
    }
    MELChar16ListPush(&characters, 0);
    return playdate->system->realloc(characters.memory, characters.count * sizeof(uint16_t));
}

uint16_t * _Nullable MELUTF16StringMakeWithUTF8String(const char * _Nullable source) {
    if (source == NULL) {
        return NULL;
    }
    MELCodePointList codePoints = MELCodePointListMakeWithUTF8String(source);
    uint16_t *utf16String = MELUTF16StringMakeWithCodePoints(codePoints);
    MELCodePointListDeinit(&codePoints);
    return utf16String;
}

unsigned int MELStringLengthToDisplayUInt(unsigned int value) {
    return value == 0
        ? 1
    : (unsigned int)log10f(value) + 1;
}

char * _Nonnull MELUInt32ToString(uint32_t value) {
    const unsigned int digitCount = MELStringLengthToDisplayUInt(value);
    char *result = playdate->system->realloc(NULL, digitCount + 1);
    result[digitCount] = '\0';
    for (int index = digitCount - 1; index >= 0; index--) {
        result[index] = '0' + (value % 10);
        value /= 10;
    }
    return result;
}

char * _Nonnull MELUInt32ToStringWithBuffer(uint32_t value, char * _Nullable buffer, int * _Nonnull bufferCapacity) {
    const unsigned int digitCount = MELStringLengthToDisplayUInt(value);
    const unsigned int expectedCapacity = digitCount + 1;
    if (buffer == NULL || expectedCapacity > *bufferCapacity) {
        buffer = playdate->system->realloc(buffer, expectedCapacity);
        *bufferCapacity = expectedCapacity;
    }
    buffer[digitCount] = '\0';
    for (int index = digitCount - 1; index >= 0; index--) {
        buffer[index] = '0' + (value % 10);
        value /= 10;
    }
    return buffer;
}

void MELUInt32ToStringWithFixedSizeBuffer(uint32_t value, char * _Nonnull buffer, const int bufferCapacity) {
    const unsigned int digitCount = MELIntMin(MELStringLengthToDisplayUInt(value), bufferCapacity - 1);
    buffer[digitCount] = '\0';
    for (int index = digitCount - 1; index >= 0; index--) {
        buffer[index] = '0' + (value % 10);
        value /= 10;
    }
}

int MELStringIndexOfString(const char * _Nonnull haystack, const char * _Nonnull needle) {
    const char *position = strstr(haystack, needle);
    if (position == NULL) {
        return -1;
    } else {
        return (int) (position - haystack);
    }
}
