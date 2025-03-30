//
//  layer.h
//  Roll
//
//  Created by Raphaël Calabro on 22/03/2022.
//

#ifndef constlayer_h
#define constlayer_h

#include "melstd.h"

#include "rectangle.h"
#include "point.h"
#include "direction.h"
#include "list.h"

#define kEmptyTile 0xFFFF

typedef struct melmap MELMap;

typedef struct mellayer {
    MELMap * _Nullable parent;
    MELIntRectangle frame;
    MELPoint scrollRate;
    MELBoolean isGround;
    int tileCount;
    uint16_t * _Nullable tiles;
} MELLayer;

typedef MELLayer * _Nullable MELLayerRef;

MELListDefine(MELLayerRef);

extern const MELLayer MELLayerEmpty;

void MELLayerDeinit(MELLayer * _Nonnull self);

uint16_t MELLayerTileAtXAndY(MELLayer * _Nonnull self, float x, float y);
MELIntPoint MELLayerPointInTileAtPoint(MELLayer * _Nonnull self, MELPoint point);

/**
 * Returns `true` if the given tile is not transparent at the given point (relative to tile top left).
 * @param self Layer instance.
 * @param tile Tile number.
 * @param pointInsideTile Location to check.
 */
MELBoolean MELLayerCollidesWithPoint(MELLayer * _Nonnull self, uint16_t tile, MELIntPoint pointInsideTile);

GLfloat MELLayerTileTop(MELLayer * _Nonnull self, MELPoint point);
GLfloat MELLayerTileBottom(MELLayer * _Nonnull self, MELPoint point);
GLfloat MELLayerTileBorder(MELLayer * _Nonnull self, MELPoint point, MELDirection direction);

/**
 * Angle at point for the given horizontal direction.
 *
 * @param self Layer instance.
 * @param point Location.
 * @param direction Left or right direction.
 * @return Angle at point for the given horizontal direction.
 */
GLfloat MELLayerAngleAtPoint(MELLayer * _Nonnull self, MELPoint point, MELDirection direction);

/**
 * Angle at point for the given vertical direction.
 *
 * @param self Layer instance.
 * @param point Location.
 * @param direction Up or down direction.
 * @return Angle at point for the given vertical direction.
 */
GLfloat MELLayerVerticalAngleAtPoint(MELLayer * _Nonnull self, MELPoint point, MELDirection direction);

#endif /* constlayer_h */
