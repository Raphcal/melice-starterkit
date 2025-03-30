//
//  crank.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 02/07/2023.
//

#ifndef crank_h
#define crank_h

#include "melstd.h"

float MELCrankGetAcceleratedChange(void);

typedef struct melcrankindicator MELCrankIndicator;
MELCrankIndicator * _Nonnull MELCrankIndicatorConstructor(MELBoolean clockwise);
void MELCrankIndicatorDealloc(MELCrankIndicator * _Nonnull self);
void MELCrankIndicatorDraw(MELCrankIndicator * _Nonnull self, unsigned int xOffset, unsigned int yOffset);

#endif /* crank_h */
