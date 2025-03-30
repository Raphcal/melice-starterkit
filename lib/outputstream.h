//
//  outputstream.h
//  melice
//
//  Created by Raphaël Calabro on 22/11/2021.
//

#ifndef outputstream_h
#define outputstream_h

#include "melstd.h"

#include "point.h"
#include "size.h"
#include "rectangle.h"
#include "hash.h"
#include "uuid.h"

#define MELOutputStreamWriteByte MELOutputStreamWriteUInt8

typedef struct {
    SDFile * _Nullable file;
    uint8_t * _Nullable buffer;
    int32_t count;
    int32_t capacity;
    MELSHA256Context * _Nullable sha256;
    int hashStart;
} MELOutputStream;

MELOutputStream MELOutputStreamOpen(const char * _Nonnull path);
int MELOutputStreamClose(MELOutputStream * _Nonnull self);

MELOutputStream MELOutputStreamInit(void);
void MELOutputStreamDeinit(MELOutputStream * _Nonnull self);

uint8_t * _Nonnull MELOutputStreamGetBytes(MELOutputStream self);

void MELOutputStreamFlush(MELOutputStream * _Nonnull self);

/**
 * Writes and returns one byte to the given output stream.
 *
 * @param self Output stream to write to.
 * @param byte Unsigned 8 bits integer to write.
 */
void MELOutputStreamWriteUInt8(MELOutputStream * _Nonnull self, uint8_t byte);

void MELOutputStreamWriteInt8(MELOutputStream * _Nonnull self, int8_t byte);

/**
 * Writes size bytes from the given source into the output stream.
 *
 * @param self Output stream to write to.
 * @param source Pointer where to read the read bytes from.
 * @param size Number of bytes to write.
 */
void MELOutputStreamWrite(MELOutputStream * _Nonnull self, const void * _Nonnull source, unsigned int size);

/**
 * Write the given number of zero.
 *
 * @param self Output stream to write to.
 * @param size Number of bytes to skip.
 */
void MELOutputStreamWriteZero(MELOutputStream * _Nonnull self, unsigned int size);

/**
 * Writes a boolean to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value Boolean value to write.
 */
void MELOutputStreamWriteBoolean(MELOutputStream * _Nonnull self, MELBoolean value);

/**
 * Writes a 32 bits signed integer to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value 32 bits signed integer to write.
 */
void MELOutputStreamWriteInt(MELOutputStream * _Nonnull self, int32_t value);

/**
 * Writes a 16 bits signed integer to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value 16 bits signed integer.
 */
void MELOutputStreamWriteShort(MELOutputStream * _Nonnull self, int16_t value);

void MELOutputStreamWriteUInt16(MELOutputStream * _Nonnull self, uint16_t value);
void MELOutputStreamWriteUInt32(MELOutputStream * _Nonnull self, uint32_t value);

/**
 * Writes a float to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value float.
 */
void MELOutputStreamWriteFloat(MELOutputStream * _Nonnull self, float value);

/**
 * Writes a double to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value double.
 */
void MELOutputStreamWriteDouble(MELOutputStream * _Nonnull self, double value);

/**
 * Writes a byte array to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value Byte array to write.
 * @param count Number of elements to write.
 */
void MELOutputStreamWriteByteArray(MELOutputStream * _Nonnull self, uint8_t * _Nonnull value, int32_t count);

/**
 * Writes a byte array to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value Byte array or NULL.
 * @param count Number of elements to write. Will be set to 0 if the array is NULL.
 *
 */
void MELOutputStreamWriteNullableByteArray(MELOutputStream * _Nonnull self, uint8_t * _Nullable value, int32_t count);

void MELOutputStreamWriteInt8Array(MELOutputStream * _Nonnull self, int8_t * _Nonnull value, int32_t count);

/**
 * Writes a 32 bits signed integer array to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value 32 bits signed integer array.
 * @param count Number of elements to write.
 */
void MELOutputStreamWriteIntArray(MELOutputStream * _Nonnull self, int32_t * _Nonnull value, int32_t count);

/**
 * Writes the given UTF-8 string as an UTF-16 string to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value UTF-8 string.
 */
void MELOutputStreamWriteString(MELOutputStream * _Nonnull self, char * _Nonnull value);

/**
 * Writes the given UTF-8 string as an UTF-16 string to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value UTF-8 string or NULL.
 */
void MELOutputStreamWriteNullableString(MELOutputStream * _Nonnull self, char * _Nullable value);

/**
 * Writes the given UTF-8 string to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value UTF-8 string.
 */
void MELOutputStreamWriteUTF8String(MELOutputStream * _Nonnull self, char * _Nonnull value);

/**
 * Writes a point to the given ouput stream.
 *
 * @param self Output stream to write to.
 * @param value Point to write.
 */
void MELOutputStreamWritePoint(MELOutputStream * _Nonnull self, MELPoint value);
void MELOutputStreamWriteSize(MELOutputStream * _Nonnull self, MELSize value);
void MELOutputStreamWriteRectangle(MELOutputStream * _Nonnull self, MELRectangle value);

void MELOutputStreamWriteIntPoint(MELOutputStream * _Nonnull self, MELIntPoint value);
void MELOutputStreamWriteIntSize(MELOutputStream * _Nonnull self, MELIntSize value);
void MELOutputStreamWriteIntRectangle(MELOutputStream * _Nonnull self, MELIntRectangle value);

void MELOutputStreamWriteUUID(MELOutputStream * _Nonnull self, MELUUID uuid);

void MELOutputStreamStartHash(MELOutputStream * _Nonnull self, const char * _Nonnull salt, const unsigned int saltLength);

void MELOutputStreamEndHash(MELOutputStream * _Nonnull self);

#endif /* outputstream_h */
