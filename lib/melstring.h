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

extern const char kEmptyString[];

MELBoolean MELStringEquals(const char * _Nonnull lhs, const char * _Nonnull rhs);
MELBoolean MELNullableStringEquals(const char * _Nullable lhs, const char * _Nullable rhs);

MELBoolean MELStringStartsWith(const char * _Nonnull lhs, const char * _Nonnull rhs);
MELBoolean MELStringEndsWith(const char * _Nonnull lhs, const char * _Nonnull rhs);

uint32_t MELStringHash(const char * _Nullable key);

char * _Nullable MELStringCopy(const char * restrict _Nullable source);
char * _Nonnull MELStringConcat(const char * _Nullable lhs, const char * _Nullable rhs);

MELCodePointList MELCodePointListMakeWithUTF8String(const char * _Nullable source);
void MELCodePointListMakeWithUTF8StringAndBuffer(const char * _Nullable source, MELCodePointList * _Nonnull codePoints);
MELCodePointList MELCodePointListMakeWithUTF16String(const uint16_t * _Nullable source);

char * _Nullable MELUTF8StringMakeWithCodePoints(MELCodePointList codePoints);
char * _Nullable MELUTF8StringMakeWithUTF16String(const uint16_t * _Nullable source);
uint32_t MELUTF8StringCodePointCount(const MELChar * _Nullable source, int length);

int MELUTF8StringCharacterSize(const char * _Nullable source, int index);

uint16_t * _Nullable MELUTF16StringMakeWithCodePoints(MELCodePointList codePoints);
uint16_t * _Nullable MELUTF16StringMakeWithUTF8String(const char * _Nullable source);

/**
 * Calcul le nombre de chiffres dans la valeur donnée.
 *
 * @param value Valeur.
 * @returns Le nombre de chiffres dans la valeur donnée.
 */
unsigned int MELStringLengthToDisplayUInt(unsigned int value);

void MELStringSet2DigitsNumber(uint8_t value, char * _Nonnull buffer);
void MELStringSet4DigitsNumber(unsigned int value, char * _Nonnull buffer);
void MELStringSetNumber(uint32_t value, unsigned int digitCount, char * _Nonnull buffer);

/**
 * Formatte la durée en seconde donnée en date.
 * Le format est en 2 métriques seulement, par exemple :
 * - heures/minutes : 12h 53m
 * - minutes/secondes : 4m 12s
 * - secondes : 23s
 * Il vaut mieux prévoir un buffer d'au moins 10 caractères. Durée max : 9999h 59m.
 */
void MELStringFormatDuration(float duration, char * _Nonnull buffer);

/**
 * Converti la valeur donnée en texte.
 *
 * @param value Nombre à convertir.
 */
char * _Nonnull MELUInt32ToString(uint32_t value);
char * _Nonnull MELUInt32ToStringWithBuffer(uint32_t value, char * _Nullable buffer, int * _Nonnull bufferCapacity);
void MELUInt32ToStringWithFixedSizeBuffer(uint32_t value, char * _Nonnull buffer, const int bufferCapacity);
void MELUInt32ToStringWithFixedSizeBufferAlignRight(uint32_t value, char * _Nonnull buffer, const int bufferCapacity);

uint32_t MELStringToUInt32(const char * _Nonnull string);

int MELStringIndexOfCharacter(const char * _Nonnull haystack, char needle);
int MELStringLastIndexOfCharacter(const char * _Nonnull haystack, char needle);

#define MELStringParseInt(source) atoi(source)
#define MELStringParseFloat(source) strtof(source, NULL)

int MELStringIndexOfString(const char * _Nonnull haystack, const char * _Nonnull needle);

#endif /* melstring_h */
