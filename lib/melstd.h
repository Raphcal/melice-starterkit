//
//  melstd.h
//  melice
//
//  Created by Raphaël Calabro on 13/03/2021.
//

#ifndef melstd_h
#define melstd_h

#define MELSPRITEINSTANCE_IGNORE_ANIMATION_NAME 0
#define MELMAP_IGNORE_TILESIZE 0
#define MELSCREEN_ORIENTATION_VERTICAL 0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "pd_api.h"

#define MELPathSeparator '/'
#define MELPathSeparatorString "/"

#ifndef __clang__
#define _Nonnull
#define _Nullable
#endif

/**
 * Interval of time in seconds.
 */
typedef float MELTimeInterval;

typedef float GLfloat;

/**
 * Boolean type.
 */
#ifndef __bool_true_false_are_defined
typedef enum {
    false,
    true
} MELBoolean;
#else
typedef bool MELBoolean;
#endif

extern PlaydateAPI * _Nullable playdate;

#endif /* melstd_h */
