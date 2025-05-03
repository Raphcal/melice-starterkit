//
//  alignment.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 14/07/2024.
//

#ifndef alignment_h
#define alignment_h

#include "melstd.h"

#include "point.h"
#include "size.h"

typedef enum {
    MELHorizontalAlignmentLeft,
    MELHorizontalAlignmentCenter,
    MELHorizontalAlignmentRight,
} MELHorizontalAlignment;

typedef enum {
    MELVerticalAlignmentTop,
    MELVerticalAlignmentMiddle,
    MELVerticalAlignmentBottom,
} MELVerticalAlignment;

MELPoint MELOriginForSizeAndAlignment(MELPoint origin, MELSize size, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);

void MELAlignmentSet(LCDSprite * _Nonnull sprite, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment, MELPoint origin);
LCDSprite * _Nonnull MELAlignmentWith(LCDSprite * _Nonnull sprite, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment, MELPoint origin);

#endif /* alignment_h */
