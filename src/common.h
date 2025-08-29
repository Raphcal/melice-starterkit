//
//  common.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/01/2023.
//

#ifndef common_h
#define common_h

#include "pd_api.h"
#include "../lib/melstd.h"

// FIXME: Restaurer le fonctionnement
#define MEL_ORIENTATION_VERTICAL false

#define VERSION_NUMBER "1.0.0"

#define CHECK_CLASS_CAST 1
#define CHECK_LAYERSPRITE_MAP_WIDTH 1

#define DEFAULT_REFRESH_RATE 50
#define DEFAULT_FRAME_TIME (1.0f / DEFAULT_REFRESH_RATE)
#define SHOW_FRAME_RATE 0

#define SCREEN_WIDTH LCD_ROWS
#define SCREEN_HEIGHT LCD_COLUMNS

#define ZINDEX_BG 0
#define ZINDEX_BG_GRID 9
#define ZINDEX_SHADOW 9
#define ZINDEX_BONUS 10
#define ZINDEX_ENEMIES 12
#define ZINDEX_PLAYER 13
#define ZINDEX_CAUGHT 14
#define ZINDEX_HANDS 15
#define ZINDEX_EXPLOSIONS 16
#define ZINDEX_CHAIN 16
#define ZINDEX_BULLETS 17
#define ZINDEX_GUI 20
#define ZINDEX_HISCORE 21
#define ZINDEX_FADE 30

#define ZINDEX_LAYER_MULTIPLIER 2

extern PlaydateAPI * _Nullable playdate;
extern LCDFont * _Nullable roobertFont;
extern int REFRESH_RATE;
extern float DELTA;

#endif /* common_h */
