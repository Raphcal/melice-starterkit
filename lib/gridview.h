//
//  gridview.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/01/2024.
//

#ifndef gridview_h
#define gridview_h

#include "melstd.h"

#include "sprite.h"
#include "point.h"
#include "size.h"
#include "bitmap.h"

typedef struct melgridview MELGridView;

typedef void (* _Nullable MELGridViewDrawCell)(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);

typedef struct {
    int top;
    int right;
    int bottom;
    int left;
} MELInset;

typedef struct melgridview {
    MELSprite super;
    MELGridViewDrawCell drawCell;
    LCDBitmapRefList images;
    MELIntSize gridSize;
    MELIntSize cellSize;
    MELIntSize cellSpacing;
    MELInset inset;
    MELIntPoint selection;
    MELPoint camera;
    MELPoint cameraFrom;
    MELPoint cameraDistance;
    float totalDistance;
    float scrollingSpeed;
    PDButtons didScroll;
    float time;
    void * _Nullable userdata;
    MELBoolean drawWasDone;
    MELBoolean disableInputs;
    MELBoolean enableScrolling;
} MELGridView;

LCDSprite * _Nonnull MELGridViewMake(MELRectangle frame, MELIntSize gridSize, MELIntSize cellSize, MELGridViewDrawCell drawCell, void * _Nullable userdata);
void MELGridViewSetSelection(LCDSprite * _Nonnull sprite, MELIntPoint selection);
void MELGridViewSetSelectionAnimated(LCDSprite * _Nonnull sprite, MELIntPoint selection, PDButtons buttons);
void MELGridViewRepaintCell(LCDSprite * _Nonnull sprite, MELIntPoint cell);
void MELGridViewRepaint(LCDSprite * _Nonnull sprite);
void MELGridViewMakeVisibleAndDraw(LCDSprite * _Nonnull sprite);
void MELGridViewSetDisableInputs(LCDSprite * _Nonnull sprite, MELBoolean disableInputs);

MELPoint MELGridViewGetSelectionCenter(MELGridView * _Nonnull self);

#endif /* gridview_h */
