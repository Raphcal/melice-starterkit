//
//  files.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 25/05/2025.
//

#ifndef files_h
#define files_h

#include "melstd.h"

#define MELPathConcat(name, parent, relative) const uint32_t name##ParentLength = (uint32_t) strlen(parent); \
const uint32_t name##RelativeLength = (uint32_t) strlen(relative); \
char name[name##ParentLength + name##RelativeLength + 2]; \
memset(name, 0, name##ParentLength + name##RelativeLength + 2);\
memcpy(name, parent, name##ParentLength); \
name[name##ParentLength] = '/'; \
memcpy(name + name##ParentLength + 1, relative, name##RelativeLength);

#define MELPathAddExtension(name, fileName, extension) const uint32_t name##FileNameLength = (uint32_t) strlen(fileName); \
const uint32_t name##ExtensionLength = (uint32_t) strlen(extension); \
char name[name##FileNameLength + name##ExtensionLength + 2]; \
memset(name, 0, name##FileNameLength + name##ExtensionLength + 2);\
memcpy(name, fileName, name##FileNameLength); \
name[name##FileNameLength] = '.'; \
memcpy(name + name##FileNameLength + 1, extension, name##ExtensionLength);

void MELFileCopy(const char * _Nonnull from, const char * _Nonnull to);

#endif /* files_h */
