//
//  inputstream.h
//  shmup
//
//  Created by Raphaël Calabro on 25/01/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef inputstream_h
#define inputstream_h

#include "melstd.h"

#include <setjmp.h>

#include "point.h"
#include "size.h"
#include "rectangle.h"
#include "hash.h"
#include "uuid.h"

#define MELInputStreamReadByte MELInputStreamReadUInt8

typedef struct {
    SDFile * _Nullable file;
    uint8_t * _Nullable buffer;
    unsigned int size;
    unsigned int cursor;
    jmp_buf * _Nullable exceptionHandler;
    MELSHA256Context * _Nullable sha256;
    int hashStart;
} MELInputStream;

typedef enum {
    MELInputStreamSeekFromStart = SEEK_SET,
    MELInputStreamSeekFromCurrent = SEEK_CUR,
    MELInputStreamSeekFromEnd = SEEK_END
} MELInputStreamSeekFrom;

MELBoolean MELFileExists(const char * _Nonnull path);

/**
 * Opens and returns an input stream on the given path with the given mode.
 *
 * @param path Path of the file to open. Relative to the current directory.
 * @return An inputstream.
 */
MELInputStream MELInputStreamOpen(const char * _Nonnull path, FileOptions mode);

/**
 * Closes the given input stream.
 * No read operation will be possible on the given input stream after calling this function.
 *
 * @param self Input stream instance.
 * @return 0 if close operation succeeded, an other value otherwise.
 */
int MELInputStreamClose(MELInputStream * _Nonnull self);

/**
 * Creates an inputstream containing a copy of the bytes from the given byte array.
 *
 * @param bytes Content of the stream.
 * @param count Number of bytes to copy.
 * @return An inputstream.
 */
MELInputStream MELInputStreamInitWithBytes(const void * _Nonnull bytes, unsigned int count);

/**
 * Free the byte buffer backing the inputstream.
 *
 * @param self Input stream instance to deinit.
 */
void MELInputStreamDeinit(MELInputStream * _Nonnull self);

/**
 * Seek the position of the current inputstream to the given offset, relative to the given position.
 *
 * @param self Input stream instance.
 * @param offset Offset to seek to.
 * @param from Start position to seek from.
 * @return 0 if the operation succeeded, an other value otherwise.
 */
int MELInputStreamSeek(MELInputStream * _Nonnull self, int offset, MELInputStreamSeekFrom from);

/**
 * Reads and returns one byte from the given input stream.
 *
 * @param self Input stream instance.
 * @return A byte or -1 if the input stream end has been reached or if the stream is closed.
 */
int MELInputStreamReadUInt8(MELInputStream * _Nonnull self);

int8_t MELInputStreamReadInt8(MELInputStream * _Nonnull self);

/**
 * Reads size bytes from the given input stream into the given pointer.
 *
 * @param self Input stream to read from.
 * @param destination Pointer where to write the read bytes.
 * @param size Number of bytes to read.
 */
void MELInputStreamRead(MELInputStream * _Nonnull self, void * _Nonnull destination, unsigned int size);

/**
 * Skip the given number of bytes.
 *
 * @param self Input stream to read from.
 * @param size Number of bytes to skip.
 */
void MELInputStreamSkipBytes(MELInputStream * _Nonnull self, unsigned int size);

/**
 * Reads and returns a boolean from the given input stream.
 *
 * @param self Input stream instance.
 * @return A boolean.
 */
MELBoolean MELInputStreamReadBoolean(MELInputStream * _Nonnull self);

/**
 * Reads and returns a 32 bits signed integer from the given input stream.
 *
 * @param self Input stream instance.
 * @return A 32 bits signed integer.
 */
int32_t MELInputStreamReadInt(MELInputStream * _Nonnull self);

uint32_t MELInputStreamReadUInt32(MELInputStream * _Nonnull self);

/**
 * Reads and returns a 16 bits signed integer from the given input stream.
 *
 * @param self Input stream instance.
 * @return A 16 bits signed integer.
 */
int16_t MELInputStreamReadShort(MELInputStream * _Nonnull self);

/**
 * Reads and returns a 16 bits unsigned integer from the given input stream.
 *
 * @param self Input stream instance.
 * @return A 16 bits unsigned integer.
 */
uint16_t MELInputStreamReadUInt16(MELInputStream * _Nonnull self);

/**
 * Reads and returns a float from the given input stream.
 *
 * @param self Input stream instance.
 * @return A float.
 */
float MELInputStreamReadFloat(MELInputStream * _Nonnull self);

/**
 * Reads and returns a double from the given input stream.
 *
 * @param self Input stream instance.
 * @return A double.
 */
double MELInputStreamReadDouble(MELInputStream * _Nonnull self);

/**
 * Reads and returns a byte array from the given input stream.
 *
 * @param self Input stream instance.
 * @param count Pointer to an integer where to write the size of the returned array.
 * @return A byte array. You are responsible for freeing the returned array.
 */
uint8_t * _Nonnull MELInputStreamReadByteArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count);

/**
 * Reads and returns a byte array from the given input stream.
 *
 * @param self Input stream instance.
 * @param count Pointer to an integer where to write the size of the returned array. Will be set to 0 if the array is NULL.
 * @return A byte array or NULL. You are responsible for freeing the returned array.
 */
uint8_t * _Nullable MELInputStreamReadNullableByteArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count);

int8_t * _Nonnull MELInputStreamReadInt8Array(MELInputStream * _Nonnull self, int32_t * _Nullable count);

uint8_t * _Nonnull MELInputStreamReadUInt8Array(MELInputStream * _Nonnull self, uint32_t * _Nonnull count);

/**
 * Reads and returns a 32 bits signed integer array from the given input stream.
 *
 * @param self Input stream instance.
 * @param count Pointer to an integer where to write the size of the returned array.
 * @return A 32 bits signed integer array. You are responsible for freeing the returned array.
 */
int32_t * _Nonnull MELInputStreamReadIntArray(MELInputStream * _Nonnull self, int32_t * _Nonnull count);

uint32_t * _Nonnull MELInputStreamReadUInt32Array(MELInputStream * _Nonnull self, uint32_t * _Nonnull count);

/**
 * Reads and returns an UTF-16 string from the given input stream and convert it to UTF-8.
 *
 * @param self Input stream instance.
 * @return An UTF-8 string. You are responsible for freeing the returned string.
 */
char * _Nonnull MELInputStreamReadString(MELInputStream * _Nonnull self);

/**
 * Reads and returns an UTF-16 string from the given input stream and convert it to UTF-8.
 *
 * @param self Input stream instance.
 * @return An UTF-8 string or NULL. You are responsible for freeing the returned string.
 */
char * _Nullable MELInputStreamReadNullableString(MELInputStream * _Nonnull self);

/**
 * Reads and returns an UTF-8 string from the given input stream.
 *
 * @param self Input stream instance.
 * @return An UTF-8 string. You are responsible for freeing the returned string.
 */
char * _Nonnull MELInputStreamReadUTF8String(MELInputStream * _Nonnull self, unsigned int * _Nullable length);

/**
 * Reads and returns a color from the given input stream.
 *
 * @param self Input stream instance.
 * @return A color. You are responsible for freeing the returned color.
 */
LCDColor MELInputStreamReadColor(MELInputStream * _Nonnull self);

/**
 * Reads and returns a point from the given input stream.
 *
 * @param self Input stream instance.
 * @return A point. You are responsible for freeing the returned point.
 */
MELPoint MELInputStreamReadPoint(MELInputStream * _Nonnull self);
MELSize MELInputStreamReadSize(MELInputStream * _Nonnull self);
MELRectangle MELInputStreamReadRectangle(MELInputStream * _Nonnull self);
MELIntPoint MELInputStreamReadIntPoint(MELInputStream * _Nonnull self);
MELIntSize MELInputStreamReadIntSize(MELInputStream * _Nonnull self);
MELIntRectangle MELInputStreamReadIntRectangle(MELInputStream * _Nonnull self);

PDScore MELInputStreamReadPDScore(MELInputStream * _Nonnull self);
PDScore * _Nonnull MELInputStreamReadPDScoreArray(MELInputStream * _Nonnull self, uint32_t * _Nullable count);

MELUUID MELInputStreamReadUUID(MELInputStream * _Nonnull self);

void MELInputStreamStartHash(MELInputStream * _Nonnull self, const char * _Nonnull salt, const unsigned int saltLength);
MELBoolean MELInputStreamEndAndValidateHash(MELInputStream * _Nonnull self);

#endif /* inputstream_h */
