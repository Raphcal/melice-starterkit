//
//  timer.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 04/03/2025.
//

#ifndef timer_h
#define timer_h

#include "melstd.h"

typedef void (* _Nullable MELTimerCallback)(void * _Nullable userdata);

LCDSprite * _Nonnull MELTimerConstructor(float delay, MELTimerCallback callback, void * _Nullable userdata);

#endif /* timer_h */
