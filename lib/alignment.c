//
//  alignment.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 14/07/2024.
//

#include "alignment.h"

#include "sprite.h"

MELPoint MELOriginForSizeAndAlignment(MELPoint origin, MELSize size, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment) {
    switch (horizontalAlignment) {
        case MELHorizontalAlignmentLeft:
            origin.x += size.width / 2.0f;
            break;
        case MELHorizontalAlignmentRight:
            origin.x -= size.width / 2.0f;
            break;
        default:
            break;
    }
    switch (verticalAlignment) {
        case MELVerticalAlignmentTop:
            origin.y += size.height / 2.0f;
            break;
        case MELVerticalAlignmentBottom:
            origin.y -= size.height / 2.0f;
            break;
        default:
            break;
    }
    return origin;
}

void MELAlignmentSet(LCDSprite * _Nonnull sprite, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment, MELPoint origin) {
    MELSprite *self = playdate->sprite->getUserdata(sprite);
    MELRectangle frame = self->frame;
    frame.origin = MELOriginForSizeAndAlignment(origin, frame.size, horizontalAlignment, verticalAlignment);
    self->frame = frame;
    playdate->sprite->moveTo(sprite, frame.origin.x, frame.origin.y);
}

LCDSprite * _Nonnull MELAlignmentWith(LCDSprite * _Nonnull sprite, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment, MELPoint origin) {
    MELAlignmentSet(sprite, horizontalAlignment, verticalAlignment, origin);
    return sprite;
}
