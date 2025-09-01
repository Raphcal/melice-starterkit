//
//  metadata.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 23/05/2025.
//

#ifndef metadata_h
#define metadata_h

#include "melstd.h"

typedef struct {
    char * _Nullable name;
    char * _Nullable author;
    char * _Nullable description;
    char * _Nullable bundleID;
    char * _Nullable imagePath;
    char * _Nullable version;
    uint32_t buildNumber;
    uint32_t pdxVersion;
    uint32_t buildTime;
} MELMetadata;

MELMetadata MELMetadataGet(void);

#endif /* metadata_h */
