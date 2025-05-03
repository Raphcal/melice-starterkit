//
//  base64.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 24/10/2024.
//

#ifndef base64_h
#define base64_h

#include "melstd.h"

char * _Nullable MELBase64Encode(const void * _Nonnull data, int length);
void * _Nullable MELBase64Decode(const char * _Nonnull data, int * _Nonnull decodedLength);

#endif /* base64_h */
