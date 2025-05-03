//
//  timer.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 04/03/2025.
//

#ifndef timer_h
#define timer_h

#include "common.h"
#include "../lib/melice.h"

typedef void (* _Nullable TimerCallback)(void * _Nullable userdata);

LCDSprite * _Nonnull TimerConstructor(float delay, TimerCallback callback, void * _Nullable userdata);

#endif /* timer_h */
