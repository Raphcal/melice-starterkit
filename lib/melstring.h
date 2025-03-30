//
//  melstring.h
//  melice
//
//  Created by Raphaël Calabro on 09/11/2021.
//

#ifndef melstring_h
#define melstring_h

#include "melstd.h"

#include "list.h"

typedef char MELChar;
MELListDefine(MELChar);

typedef uint16_t MELChar16;
MELListDefine(MELChar16);

typedef uint32_t MELCodePoint;
MELListDefine(MELCodePoint);

MELBoolean MELStringEquals(const char * _Nonnull lhs, const char * _Nonnull rhs);
MELBoolean MELStringStartsWith(const char * _Nonnull lhs, const char * _Nonnull rhs);
MELBoolean MELStringEndsWith(const char * _Nonnull lhs, const char * _Nonnull rhs);

uint64_t MELStringHash(const char * _Nonnull key);

char * _Nullable MELStringCopy(const char * restrict _Nullable source);
char * _Nonnull MELStringConcat(const char * _Nullable lhs, const char * _Nullable rhs);

MELCodePointList MELCodePointListMakeWithUTF8String(char * _Nullable source);
MELCodePointList MELCodePointListMakeWithUTF16String(uint16_t * _Nullable source);

char * _Nullable MELUTF8StringMakeWithCodePoints(MELCodePointList codePoints);
char * _Nullable MELUTF8StringMakeWithUTF16String(uint16_t * _Nullable source);

uint16_t * _Nullable MELUTF16StringMakeWithCodePoints(MELCodePointList codePoints);
uint16_t * _Nullable MELUTF16StringMakeWithUTF8String(char * _Nullable source);

/**
 * Calcul le nombre de chiffres dans la valeur donnée.
 *
 * @param value Valeur.
 * @returns Le nombre de chiffres dans la valeur donnée.
 */
unsigned int MELStringLengthToDisplayUInt(unsigned int value);

/**
 * Converti la valeur donnée en texte.
 *
 * @param value Nombre à convertir.
 */
char * _Nonnull MELUInt32ToString(uint32_t value);
char * _Nonnull MELUInt32ToStringWithBuffer(uint32_t value, char * _Nullable buffer, int * _Nonnull bufferCapacity);
void MELUInt32ToStringWithFixedSizeBuffer(uint32_t value, char * _Nonnull buffer, const int bufferCapacity);

#define MELStringIndexOfCharacter(haystack, needle) strchr(haystack, needle)
#define MELStringIndexOfString(haystack, needle) strstr(haystack, needle)
#define MELStringLastIndexOfCharacter(haystack, needle) strrchr(haystack, needle)
#define MELStringParseInt(source) atoi(source)
#define MELStringParseFloat(source) strtof(source, NULL)

#endif /* melstring_h */
