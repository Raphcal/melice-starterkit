//
//  files.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 25/05/2025.
//

#include "files.h"

#define kBufferSize 4096

void MELFileCopy(const char * _Nonnull from, const char * _Nonnull to) {
    SDFile *source = playdate->file->open(from, kFileRead);
    SDFile *destination = playdate->file->open(to, kFileWrite);

    uint8_t buffer[kBufferSize] = {};
    int read = playdate->file->read(source, buffer, kBufferSize);
    while (read > 0) {
        while (read > 0) {
            read -= playdate->file->write(destination, buffer, read);
        }
        read = playdate->file->read(source, buffer, kBufferSize);
    }

    playdate->file->close(source);
    playdate->file->close(destination);
}
