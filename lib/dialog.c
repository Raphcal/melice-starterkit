//
//  dialog.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/07/2024.
//

#include "dialog.h"

#include "sprite.h"
#include "gridview.h"
#include "melmath.h"
#include "controller.h"

static void dealloc(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void updateAnimating(LCDSprite * _Nonnull sprite);
static const char * _Nonnull getChoice(MELDialogConfig * _Nonnull self, int index);
static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected);

static const MELSpriteClass MELDialogClass = (MELSpriteClass) {
    .destroy = dealloc,
};

#define bufferCount 50
static char buffer[bufferCount];

#define ZINDEX_DIALOG 90
#define ZINDEX_DIALOG_BUTTON 91

LCDSprite * _Nonnull MELDialogMake(MELDialogConfig config) {
    MELDialog *self = new(MELDialog);
    const struct playdate_graphics *graphics = playdate->graphics;

    MELIntSize border = config.border ? config.border->minSize : MELIntSizeZero;

    MELIntPoint textTopLeft = (MELIntPoint) {
        .x = config.padding.left,
        .y = config.padding.top,
    };

    if (config.selectionBorder) {
        MELIntSize topLeftSliceSize = LCDBitmapGetSize(config.selectionBorder->slices[0]);
        textTopLeft.x += topLeftSliceSize.width;
        textTopLeft.y += topLeftSliceSize.height;
    }

    MELIntSize cellSize = MELIntSizeZero;
    if (config.frame.size.width == 0) {
        for (unsigned int index = 0; index < config.count; index++) {
            const char *choice = getChoice(&config, index);
            const int width = graphics->getTextWidth(config.font, choice, 99, kUTF8Encoding, 0);
            if (width > config.frame.size.width) {
                config.frame.size.width = width;
            }
        }
        config.frame.size.width += config.padding.left + config.padding.right;
        if (config.selectionBorder) {
            config.frame.size.width += config.selectionBorder->minSize.width;
        }
        cellSize.width = config.frame.size.width;
        config.frame.size.width += border.width;
    } else {
        cellSize.width = config.frame.size.width - border.width;
    }
    // TODO: Gérer les largeurs supérieurs à la taille de l'écran.
    cellSize.height = graphics->getFontHeight(config.font) + config.padding.top + config.padding.bottom;
    if (config.selectionBorder) {
        cellSize.height += config.selectionBorder->minSize.height;
    }
    if (config.frame.size.height == 0) {
        config.frame.size.height = MELIntMin(cellSize.height * config.count + border.height, LCD_ROWS - 16);
    }
    if (config.frame.origin.x == 0) {
        config.frame.origin.x = LCD_COLUMNS / 2;
    } else if (config.frame.origin.x + config.frame.size.width / 2 > LCD_COLUMNS) {
        config.frame.origin.x = LCD_COLUMNS - config.frame.size.width / 2;
    }
    if (config.frame.origin.y == 0) {
        config.frame.origin.y = LCD_ROWS / 2;
    } else if (config.frame.origin.y + config.frame.size.height / 2 > LCD_ROWS) {
        config.frame.origin.y = LCD_ROWS - config.frame.size.height / 2;
    }

    // Image
    LCDBitmap *image = graphics->newBitmap(config.frame.size.width, 1.0f, kColorClear);

    *self = (MELDialog) {
        .super = {
            .class = &MELDialogClass,
            .frame = config.frame,
        },
        .config = config,
        .state = MELDialogStateOpening,
        .textTopLeft = textTopLeft,
    };
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUpdateFunction(sprite, updateAnimating);
    playdate->sprite->setZIndex(sprite, ZINDEX_DIALOG);
    playdate->sprite->moveTo(sprite, config.frame.origin.x - camera.frame.origin.x, config.frame.origin.y - camera.frame.origin.y);
    playdate->sprite->addSprite(sprite);

    // GridView
    // TODO: Supporter un mode horizontal pour les boutons.
    LCDSprite *gridViewSprite = MELGridViewMake((MELRectangle) {
        // TODO: Gérer le décalage dû aux bordures, il faudra sûrement ajouter à x : gauche - droite et pareil pour y.
        .origin = config.frame.origin,
        .size = {
            .width = config.frame.size.width - border.width,
            .height = config.frame.size.height - border.height,
        }
    }, MELIntSizeMake(1, config.count), cellSize, drawCell, self);
    playdate->sprite->setZIndex(gridViewSprite, ZINDEX_DIALOG_BUTTON);

    MELGridView *gridView = playdate->sprite->getUserdata(gridViewSprite);
    if (config.initialSelection) {
        MELGridViewSetSelection(gridViewSprite, (MELIntPoint) { .y = config.initialSelection });
    }
    gridView->disableInputs = true;
    playdate->sprite->setVisible(gridViewSprite, false);

    self->gridView = gridView;
    self->gridViewSprite = gridViewSprite;

    return sprite;
}

unsigned int MELDialogGetSelection(LCDSprite * _Nonnull sprite) {
    MELDialog *self = playdate->sprite->getUserdata(sprite);
    // TODO: Supporter un mode horizontal pour les boutons.
    return self->gridView->selection.y;
}

void MELDialogClose(LCDSprite * _Nonnull sprite) {
    MELDialog *self = playdate->sprite->getUserdata(sprite);
    self->state = MELDialogStateClosing;
    self->time = 0.0f;
    self->gridView->disableInputs = true;
    playdate->sprite->setVisible(self->gridViewSprite, false);
    playdate->sprite->setUpdateFunction(sprite, updateAnimating);
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    MELDialog *self = playdate->sprite->getUserdata(sprite);
    playdate->sprite->setUpdateFunction(self->gridViewSprite, self->gridView->super.class->destroy);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    MELSpriteDealloc(sprite);
}

static const char * _Nonnull getChoice(MELDialogConfig * _Nonnull self, int index) {
    const char *text;
    if (self->choices) {
        text = self->choices[index];
    } else if (self->getChoiceLabel) {
        self->getChoiceLabel(self->userdata, index, buffer, bufferCount);
        text = buffer;
    } else {
        playdate->system->logToConsole("[WARN] MELDialog: choices and getChoiceLabel are both NULL");
        buffer[0] = '-';
        buffer[1] = '\0';
        text = buffer;
    }
    return text;
}

static void draw(MELDialog * _Nonnull self, LCDSprite * _Nonnull sprite, float height) {
    const struct playdate_graphics *graphics = playdate->graphics;
    const float width = self->super.frame.size.width;

    LCDBitmap *image = playdate->sprite->getImage(sprite);
    // FIXME: Simplement dessiner le cadre au milieu de l'image à la bonne taille plutôt que désallouer/réallouer l'image.
    const float oldHeight = LCDBitmapGetHeight(image);
    if (height != oldHeight) {
        graphics->freeBitmap(image);
        image = graphics->newBitmap(width, height, kColorClear);
    }
    graphics->pushContext(image);
    if (self->config.border) {
        MELNineSliceDrawInRect(self->config.border, (MELRectangle) {
            .size = {
                .width = width,
                .height = height
            }
        });
    }
    graphics->popContext();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setSize(sprite, width, height);
    playdate->sprite->markDirty(sprite);
}

static void updateAnimating(LCDSprite * _Nonnull sprite) {
    MELDialog *self = playdate->sprite->getUserdata(sprite);
    float fromHeight, toHeight;
    if (self->state == MELDialogStateOpening) {
        fromHeight = 1.0f;
        toHeight = self->config.frame.size.height;
    } else {
        fromHeight = self->config.frame.size.height;
        toHeight = 1.0f;
    }
    // during
    const float duration = 0.25f;
    if (self->time < duration) {
        const float newTime = MELFloatMin(self->time + DELTA, duration);
        self->time = newTime;

        const float progress = MELEaseInOut(0.0f, duration, newTime);
        const float height = fromHeight + (toHeight - fromHeight) * progress;
        draw(self, sprite, height);
        return;
    }
    self->time = 0.0f;
    self->state++;
    if (self->state == MELDialogStateOpened) {
        draw(self, sprite, toHeight);
        self->gridView->disableInputs = false;
        playdate->sprite->setVisible(self->gridViewSprite, true);
        playdate->sprite->setUpdateFunction(sprite, update);
    } else if (self->state == MELDialogStateClosed) {
        playdate->sprite->setVisible(sprite, false);
        playdate->sprite->setUpdateFunction(sprite, MELSpriteNoopUpdate);
    }
}

static void update(LCDSprite * _Nonnull sprite) {
    MELDialog *self = playdate->sprite->getUserdata(sprite);
    // TODO: Gérer l'animation d'apparition puis l'affichage de la grille.
    if (self->time < 1.0f) {
        self->time += DELTA;
        return;
    }
    self->gridView->disableInputs = false;
}

static void drawCell(MELGridView * _Nonnull gridView, LCDBitmap * _Nullable image, int x, int y, MELBoolean isSelected) {
    MELDialog *self = gridView->userdata;

    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    if (isSelected && self->config.selectionBorder) {
        const MELIntSize cellSize = gridView->cellSize;
        MELNineSliceDrawInRect(self->config.selectionBorder, (MELRectangle) {
            .size = {
                .width = cellSize.width,
                .height = cellSize.height,
            }
        });
    }

    const char *choice = getChoice(&self->config, y);
    const MELIntPoint textTopLeft = self->textTopLeft;

    graphics->setFont(self->config.font);
    // TODO: Gérer un alignement.
    graphics->drawText(choice, 99, kUTF8Encoding, textTopLeft.x, textTopLeft.y);

    graphics->popContext();
}
