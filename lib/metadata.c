//
//  metadata.c
//  Montgolfier
//
//  Created by Raphaël Calabro on 23/05/2025.
//

#include "metadata.h"

#include "inputstream.h"
#include "melstring.h"

#define MELMetadataBufferSize 255

static MELMetadata instance = (MELMetadata) {};

typedef enum {
    MELMetadataNameNone,
    MELMetadataNameName,
    MELMetadataNameAuthor,
    MELMetadataNameDescription,
    MELMetadataNameBundleID,
    MELMetadataNameImagePath,
    MELMetadataNameVersion,
    MELMetadataNameBuildNumber,
    MELMetadataNamePdxVersion,
    MELMetadataNameBuildTime,
    MELMetadataNameCount,
    MELMetadataNameUnknown,
} MELMetadataName;

static const char * _Nonnull MELMetadataNameValues[] = {
    "", "name", "author", "description", "bundleid", "imagepath", "version", "buildnumber", "pdxversion", "buildtime"
};

static void loadMetadata(void) {
    MELInputStream inputStream = MELInputStreamOpen("pdxinfo", kFileRead);
    if (inputStream.file == NULL) {
        playdate->system->logToConsole("Unable to open pdxinfo");
        MELInputStreamClose(&inputStream);
        return;
    }

    MELMetadata metadata = (MELMetadata) {};
    MELMetadataName name = MELMetadataNameNone;
    char buffer[MELMetadataBufferSize] = {};
    int index = 0;
    for (int c = MELInputStreamReadInt8(&inputStream); c != -1; c = MELInputStreamReadInt8(&inputStream)) {
        if (c == '=' && name == MELMetadataNameNone) {
            buffer[index] = '\0';
            index = 0;
            name = MELMetadataNameUnknown;
            for (MELMetadataName aName = MELMetadataNameName; aName < MELMetadataNameCount; aName++) {
                if (!strcmp(buffer, MELMetadataNameValues[aName])) {
                    name = aName;
                    break;
                }
            }
        } else if (c == '\r' || c == '\n') {
            buffer[index] = '\0';
            index = 0;
            switch (name) {
                case MELMetadataNameName:
                    metadata.name = MELStringCopy(buffer);
                    break;
                case MELMetadataNameAuthor:
                    metadata.author = MELStringCopy(buffer);
                    break;
                case MELMetadataNameDescription:
                    metadata.description = MELStringCopy(buffer);
                    break;
                case MELMetadataNameBundleID:
                    metadata.bundleID = MELStringCopy(buffer);
                    break;
                case MELMetadataNameImagePath:
                    metadata.imagePath = MELStringCopy(buffer);
                    break;
                case MELMetadataNameVersion:
                    metadata.version = MELStringCopy(buffer);
                    break;
                case MELMetadataNameBuildNumber:
                    metadata.buildNumber = MELStringToUInt32(buffer);
                    break;
                case MELMetadataNamePdxVersion:
                    metadata.pdxVersion = MELStringToUInt32(buffer);
                    break;
                case MELMetadataNameBuildTime:
                    metadata.buildTime = MELStringToUInt32(buffer);
                    break;
                default:
                    // Pas de traitement
                    break;
            }
            name = MELMetadataNameNone;
        } else if (index < MELMetadataBufferSize - 1) {
            buffer[index++] = name == MELMetadataNameNone && c >= 'A' && c <= 'Z'
                ? c - ('A' - 'a')
                : c;
        }
    }
    instance = metadata;
    MELInputStreamClose(&inputStream);
}

MELMetadata MELMetadataGet(void) {
    if (instance.bundleID == NULL) {
        loadMetadata();
    }
    return instance;
}
