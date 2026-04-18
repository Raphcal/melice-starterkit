//
//  screen.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 25/04/2023.
//

#include "screen.h"

const MELRectangle MELScreen = (MELRectangle) {
#if MELSCREEN_ORIENTATION_VERTICAL
    .origin = {
        .x = LCD_ROWS / 2,
        .y = LCD_COLUMNS / 2,
    },
    .size = {
        .width = LCD_ROWS,
        .height = LCD_COLUMNS,
    }
#else
    .origin = {
        .x = LCD_COLUMNS / 2,
        .y = LCD_ROWS / 2,
    },
    .size = {
        .width = LCD_COLUMNS,
        .height = LCD_ROWS,
    }
#endif
};
