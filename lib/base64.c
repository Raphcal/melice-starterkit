//
//  base64.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 24/10/2024.
//

#include "base64.h"

static const char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char kPadding = '=';

static const int kTripletLength = 3;
static const int kEncodedTripletLength = 4;

#define MELBASE64_BUFFER_LENGTH 64
static uint8_t kBuffer[MELBASE64_BUFFER_LENGTH];

char * _Nonnull MELBase64Encode(const void * _Nonnull data, int length) {
    const uint8_t *input = data;
    // Chaque triplet (= 3 octets) est encodé en 4 octets.
    const int outputLength = kEncodedTripletLength * ((length + (kTripletLength - 1)) / kTripletLength);
    kBuffer[outputLength] = '\0';

    for (int inputIndex = 0, outputIndex = 0; inputIndex < length; inputIndex += kTripletLength, outputIndex += kEncodedTripletLength) {
        uint32_t triplet;

        switch (length - inputIndex) {
            case 1:
                triplet = input[inputIndex] << 16;
                break;

            case 2:
                triplet = (input[inputIndex] << 16) | (input[inputIndex + 1] << 8);
                break;
                
            default:
                triplet = (input[inputIndex] << 16) | (input[inputIndex + 1] << 8) | input[inputIndex + 2];
                break;
        }
        kBuffer[outputIndex] = kTable[(triplet >> 18) & 0x3F];
        kBuffer[outputIndex + 1] = kTable[(triplet >> 12) & 0x3F];
        kBuffer[outputIndex + 2] = kTable[(triplet >> 6) & 0x3F];
        kBuffer[outputIndex + 3] = kTable[triplet & 0x3F];
    }
    switch (length % kTripletLength) {
        case 1:
            kBuffer[outputLength - 2] = kPadding;
        case 2:
            kBuffer[outputLength - 1] = kPadding;
        default:
            break;
    }
    return (char *) kBuffer;
}

static int charToValue(char c) {
    switch (c) {
        case 'A' ... 'Z':
            return c - 'A';
        case 'a' ... 'z':
            return c - 'a' + 26;
        case '0' ... '9':
            return c - '0' + 52;
        case '+':
            return 62;
        case '/':
            return 63;
        case '=':
            return 0;
        default:
            playdate->system->logToConsole("Invalid base64 character: %c", c);
            return 0;
    }
}

void * _Nullable MELBase64Decode(const char * _Nonnull input, int * _Nonnull _outputLength) {
    const uint32_t inputLength = (uint32_t)strlen(input);
    if (inputLength % 4 != 0) {
        // La longueur de l'entrée doit être un multiple de 4
        return NULL;
    }

    const int outputLength = (inputLength / kEncodedTripletLength) * kTripletLength
        - (input[inputLength - 2] == kPadding)
        - (input[inputLength - 1] == kPadding);
    *_outputLength = outputLength;

    if (outputLength > MELBASE64_BUFFER_LENGTH) {
        playdate->system->error("Data to decode is to big! Expected at most %d but was: %d", MELBASE64_BUFFER_LENGTH, outputLength);
        return NULL;
    }

    for (int inputIndex = 0, outputIndex = 0; inputIndex < inputLength; inputIndex += kEncodedTripletLength, outputIndex += kTripletLength) {
        const uint8_t first = charToValue(input[inputIndex]);
        const uint8_t second = charToValue(input[inputIndex + 1]);
        const uint8_t third = charToValue(input[inputIndex + 2]);
        const uint8_t fourth = charToValue(input[inputIndex + 3]);

        const uint32_t triplet = (first << 18) | (second << 12) | (third << 6) | fourth;

        switch (outputLength - outputIndex) {
            case 1:
                kBuffer[outputIndex] = (triplet >> 16) & 0xFF;
                break;
            case 2:
                kBuffer[outputIndex] = (triplet >> 16) & 0xFF;
                kBuffer[outputIndex + 1] = (triplet >> 8) & 0xFF;
                break;
            default:
                kBuffer[outputIndex] = (triplet >> 16) & 0xFF;
                kBuffer[outputIndex + 1] = (triplet >> 8) & 0xFF;
                kBuffer[outputIndex + 2] = triplet & 0xFF;
                break;
        }
    }

    return kBuffer;
}

#pragma mark - Tests unitaires


