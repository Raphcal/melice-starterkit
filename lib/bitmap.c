//
//  bitmap.c
//  Roll
//
//  Created by Raphaël Calabro on 17/10/2022.
//

#include "bitmap.h"

typedef struct {
    char magic[2];
    int32_t fileSize;
    int32_t reserved;
    int32_t contentPosition;
} MELBitmapFileHeader;

typedef struct {
    int32_t headerLength;
    int16_t width;
    int16_t height;
    int16_t planes;
    int16_t bitsPerPixel;
} MELBitmapCoreHeader;

static uint8_t bayerMatrix[8][8] = {
    {0, 48, 12, 60, 3, 51, 15, 63},
    {32, 16, 44, 28, 35, 19, 47, 31},
    {8, 56, 4, 52, 11, 59, 7, 55},
    {40, 24, 36, 20, 43, 27, 39, 23},
    {2, 50, 14, 62, 1, 49, 13, 61},
    {34, 18, 46, 30, 33, 17, 45, 29},
    {10, 58, 6, 54, 9, 57, 5, 53},
    {42, 26, 38, 22, 41, 25, 37, 21}
};

void MELOutputStreamWriteBitmap(MELOutputStream * _Nonnull outputStream, LCDBitmap * _Nonnull bitmap) {
    const int32_t fileHeaderLength = 14;
    const int32_t coreHeaderLength = 12;
    const int32_t bitsInAByte = 8;
    const int32_t bytesPerColors = 3;

    int width, height, rowBytes;
    uint8_t *data;
    playdate->graphics->getBitmapData(bitmap, &width, &height, &rowBytes, NULL, &data);

    MELBitmapFileHeader fileHeader = (MELBitmapFileHeader) {
        .magic = {'B', 'M'},
        .fileSize = fileHeaderLength + coreHeaderLength + width * height * sizeof(uint8_t) * bytesPerColors,
        .contentPosition = fileHeaderLength + coreHeaderLength
    };
    MELBitmapCoreHeader coreHeader = (MELBitmapCoreHeader) {
        .headerLength = coreHeaderLength,
        .width = width,
        .height = height,
        .planes = 1,
        .bitsPerPixel = bitsInAByte * bytesPerColors
    };
    MELOutputStreamWriteByte(outputStream, fileHeader.magic[0]);
    MELOutputStreamWriteByte(outputStream, fileHeader.magic[1]);
    MELOutputStreamWriteInt(outputStream, fileHeader.fileSize);
    MELOutputStreamWriteInt(outputStream, fileHeader.reserved);
    MELOutputStreamWriteInt(outputStream, fileHeader.contentPosition);
    MELOutputStreamWriteInt(outputStream, coreHeader.headerLength);
    MELOutputStreamWriteShort(outputStream, coreHeader.width);
    MELOutputStreamWriteShort(outputStream, coreHeader.height);
    MELOutputStreamWriteShort(outputStream, coreHeader.planes);
    MELOutputStreamWriteShort(outputStream, coreHeader.bitsPerPixel);

    const int32_t count = width * height;
    for (int32_t index = 0; index < count; index++) {
        const int32_t x = index % width;
        const int32_t y = height - (index / width) - 1;

        const int32_t byteIndex = x / bitsInAByte + y * rowBytes;
        const int bitIndex = (1 << (bitsInAByte - 1)) >> (x % bitsInAByte);

        const uint8_t color = data[byteIndex] & bitIndex ? 0xFF : 0x00;
        MELOutputStreamWriteByte(outputStream, color); // Red
        MELOutputStreamWriteByte(outputStream, color); // Green
        MELOutputStreamWriteByte(outputStream, color); // Blue
    }
}

LCDBitmap * _Nonnull LCDBitmapLoadOrError(const char * _Nonnull path) {
    const char *error = NULL;
    LCDBitmap *bitmap = playdate->graphics->loadBitmap(path, &error);
    if (error) {
        playdate->system->logToConsole("Unable to load bitmap at path %s, error: %s", path, error);

        error = NULL;
        bitmap = playdate->graphics->loadBitmap(path, &error);
        if (error) {
            playdate->system->error("Unable to load bitmap at path %s, error: %s", path, error);
        }
    }
    return bitmap;
}

LCDBitmapTable * _Nonnull LCDBitmapTableLoadOrError(const char * _Nonnull path) {
    const char *error = NULL;
    LCDBitmapTable *bitmapTable = playdate->graphics->loadBitmapTable(path, &error);
    if (error) {
        playdate->system->logToConsole("Unable to load bitmap table at path %s, error: %s", path, error);

        error = NULL;
        bitmapTable = playdate->graphics->loadBitmapTable(path, &error);
        if (error) {
            playdate->system->error("Unable to load bitmap table at path %s, error: %s", path, error);
        }
    }
    return bitmapTable;
}

LCDFont * _Nonnull LCDFontLoadOrError(const char * _Nonnull path) {
    const char *error = NULL;
    LCDFont *font = playdate->graphics->loadFont(path, &error);
    if (error) {
        playdate->system->logToConsole("Unable to load font at path %s: %s", path, error);

        error = NULL;
        font = playdate->graphics->loadFont(path, &error);
        if (error) {
            playdate->system->error("Unable to load font at path %s: %s", path, error);
        }
    }
    return font;
}

void LCDBitmapFadeImage(LCDBitmap * _Nonnull bitmap, uint8_t value) {
    int width, height, rowBytes;
    uint8_t *data, *mask;
    playdate->graphics->getBitmapData(bitmap, &width, &height, &rowBytes, &mask, &data);

    const uint8_t threshold = (value * 64) / 100;
    const unsigned int size = width * height;
    for (unsigned int index = 0; index < size; index++) {
        const int x = index % width;
        const int y = index / width;

        // Appliquer la matrice de Bayer à l'emplacement courant
        const uint8_t bayerValue = bayerMatrix[y % 8][x % 8];

        // Comparer la valeur du pixel avec le seuil pour la conversion
        if (bayerValue < threshold) {
            const int32_t bitsInAByte = 8;
            const int32_t byteIndex = x / bitsInAByte + y * rowBytes;
            const int bitIndex = (1 << (bitsInAByte - 1)) >> (x % bitsInAByte);

            data[byteIndex] = data[byteIndex] & ~bitIndex;
            mask[byteIndex] = mask[byteIndex] | bitIndex;
        }
    }
}

int LCDBitmapGetWidth(LCDBitmap * _Nonnull bitmap) {
    int width;
    playdate->graphics->getBitmapData(bitmap, &width, NULL, NULL, NULL, NULL);
    return width;
}
