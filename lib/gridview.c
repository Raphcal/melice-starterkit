//
//  gridview.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 21/01/2024.
//

#include "gridview.h"

#include "melmath.h"
#include "camera.h"

static void destroy(LCDSprite * _Nonnull sprite);
static void update(LCDSprite * _Nonnull sprite);
static void updateScrolling(LCDSprite * _Nonnull sprite);
static void updateScrollRepeat(LCDSprite * _Nonnull sprite);
static void draw(MELGridView * _Nonnull self, LCDBitmap * _Nonnull image);
static MELBoolean isCellPartiallyVisible(MELGridView * _Nonnull self, MELIntPoint cell);
static MELBoolean isCellVisible(MELGridView * _Nonnull self, MELIntPoint cell);
static void setSelection(MELGridView * _Nonnull self, LCDSprite * _Nonnull sprite, MELIntPoint selection);

/**
 * Vitesse en pixel par seconde pour le scrolling.
 */
static const float kDefaultScrollingSpeed = 800.0f;

static const MELSpriteClass MELGridViewClass = (MELSpriteClass) {
    .destroy = destroy,
    .update = update,
};

LCDSprite * _Nonnull MELGridViewMake(MELRectangle frame, MELIntSize gridSize, MELIntSize cellSize, MELGridViewDrawCell drawCell, void * _Nullable userdata) {
    MELGridView *self = playdate->system->realloc(NULL, sizeof(MELGridView));

    *self = (MELGridView) {
        .super = {
            .class = &MELGridViewClass,
            .frame = frame,
        },
        .gridSize = gridSize,
        .cellSize = cellSize,
        .drawCell = drawCell,
        .userdata = userdata,
        .scrollingSpeed = kDefaultScrollingSpeed,
        .enableScrolling = true,
    };

    LCDBitmapRefList images = LCDBitmapRefListMakeWithInitialCapacity(gridSize.width * gridSize.height);
    images.count = images.capacity;
    memset(images.memory, 0, sizeof(LCDBitmapRef) * images.count);
    self->images = images;

    LCDBitmap *image = playdate->graphics->newBitmap(frame.size.width, frame.size.height, kColorClear);
    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, image, kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setUpdateFunction(sprite, update);
    playdate->sprite->moveTo(sprite, frame.origin.x - camera.frame.origin.x, frame.origin.y - camera.frame.origin.y);
    playdate->sprite->addSprite(sprite);
    return sprite;
}

void MELGridViewSetSelection(LCDSprite * _Nonnull sprite, MELIntPoint selection) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    setSelection(self, sprite, selection);
    if (!isCellVisible(self, selection)) {
        const MELRectangle frame = self->super.frame;
        const MELIntSize gridSize = self->gridSize;
        const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
        self->camera = (MELPoint) {
            .x = MELFloatMin(selection.x * cellSize.width, gridSize.width * cellSize.width - frame.size.width),
            .y = MELFloatMin(selection.y * cellSize.height, gridSize.height * cellSize.height - frame.size.height),
        };
    }
}

void MELGridViewRepaintCell(LCDSprite * _Nonnull sprite, MELIntPoint cell) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    const unsigned int index = cell.y * self->gridSize.width + cell.x;
    const MELBoolean isSelected = MELIntPointEquals(self->selection, cell);
    LCDBitmap *bitmap = self->images.memory[index];
    if (bitmap != NULL) {
        self->drawCell(self, bitmap, cell.x, cell.y, isSelected);
    }
    if (isCellPartiallyVisible(self, cell)) {
        draw(self, playdate->sprite->getImage(sprite));
        playdate->sprite->markDirty(sprite);
    }
}

void MELGridViewRepaint(LCDSprite * _Nonnull sprite) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    const MELIntSize gridSize = self->gridSize;
    const MELIntPoint selection = self->selection;
    LCDBitmapRefList images = self->images;
    for (unsigned int index = 0; index < images.count; index++) {
        const unsigned int x = index % gridSize.width;
        const unsigned int y = index / gridSize.width;
        const MELBoolean isSelected = selection.x == x && selection.y == y;

        LCDBitmap *bitmap = self->images.memory[index];
        if (bitmap != NULL) {
            self->drawCell(self, bitmap, x, y, isSelected);
        }
    }
    draw(self, playdate->sprite->getImage(sprite));
    playdate->sprite->markDirty(sprite);
}

void MELGridViewMakeVisibleAndDraw(LCDSprite * _Nonnull sprite) {
    const struct playdate_sprite *spriteAPI = playdate->sprite;
    MELGridView *self = spriteAPI->getUserdata(sprite);
    spriteAPI->setVisible(sprite, true);
    self->disableInputs = false;
    if (self->drawWasDone) {
        return;
    }
    draw(self, spriteAPI->getImage(sprite));
    self->drawWasDone = true;
}

void MELGridViewSetDisableInputs(LCDSprite * _Nonnull sprite, MELBoolean disableInputs) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    self->disableInputs = disableInputs;
}

MELPoint MELGridViewGetSelectionCenter(MELGridView * _Nonnull self) {
    const MELIntPoint selection = self->selection;
    const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
    const MELInset inset = self->inset;
    const MELPoint camera = self->camera;
    return (MELPoint) {
        .x = selection.x * cellSize.width + inset.left + self->cellSize.width / 2 - camera.x,
        .y = selection.y * cellSize.height + inset.top + self->cellSize.height / 2 - camera.y,
    };
}

static void destroy(LCDSprite * _Nonnull sprite) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    LCDBitmapRefListDeinitAndFreeBitmaps(&self->images);
    LCDBitmap *image = playdate->sprite->getImage(sprite);
    playdate->graphics->freeBitmap(image);
    MELSpriteDealloc(sprite);
}

static void draw(MELGridView * _Nonnull self, LCDBitmap * _Nonnull image) {
    const struct playdate_graphics *graphics = playdate->graphics;
    graphics->setDrawMode(kDrawModeCopy);
    graphics->clearBitmap(image, kColorClear);
    graphics->pushContext(image);

    LCDBitmapRefList images = self->images;

    const MELIntSize gridSize = self->gridSize;
    const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
    const MELPoint camera = self->camera;
    const MELInset inset = self->inset;
    const MELIntPoint topLeft = (MELIntPoint) {
        .x = camera.x - inset.left,
        .y = camera.y - inset.top
    };

    const MELIntSize scrolling = {topLeft.x % cellSize.width, topLeft.y % cellSize.height};

    const MELIntPoint topLeftTile = (MELIntPoint) {
        .x = topLeft.x / cellSize.width,
        .y = topLeft.y / cellSize.height,
    };
    const int firstTile = topLeftTile.y * gridSize.width + topLeftTile.x;

    const MELSize size = self->super.frame.size;
    const MELIntSize tileCountInScreen = (MELIntSize) {
        (int) ceilf(size.width / cellSize.width) + 1,
        (int) ceilf(size.height / cellSize.height) + 1
    };

    const int length = tileCountInScreen.width * tileCountInScreen.height;
    for (int index = 0; index < length; index++) {
        const int x = index % tileCountInScreen.width;
        const int y = index / tileCountInScreen.width;
        const int tile = firstTile + y * gridSize.width + x;

        if (x + topLeftTile.x >= 0
            && x + topLeftTile.x < gridSize.width
            && y + topLeftTile.y >= 0
            && y + topLeftTile.y < gridSize.height) {
            LCDBitmap *bitmap = images.memory[tile];
            if (bitmap == NULL) {
                const MELIntPoint cell = (MELIntPoint) { .x = topLeftTile.x + x, .y = topLeftTile.y + y };
                const MELBoolean isSelected = MELIntPointEquals(cell, self->selection);
                images.memory[tile] = bitmap = playdate->graphics->newBitmap(cellSize.width, cellSize.height, kColorClear);
                self->drawCell(self, bitmap, cell.x, cell.y, isSelected);
            }
            graphics->drawBitmap(bitmap, x * cellSize.width - scrolling.width, y * cellSize.height - scrolling.height, kBitmapUnflipped);
        }
    }
    graphics->popContext();
}

static MELIntPoint nextSelection(MELGridView * _Nonnull self, PDButtons pressed) {
    MELIntPoint selection = self->selection;
    const MELIntSize gridSize = self->gridSize;
    if ((pressed & kButtonLeft) && selection.x > 0) {
        selection.x--;
    }
    else if ((pressed & kButtonRight) && selection.x < gridSize.width - 1) {
        selection.x++;
    }
    if ((pressed & kButtonUp) && selection.y > 0) {
        selection.y--;
    }
    else if ((pressed & kButtonDown) && selection.y < gridSize.height - 1) {
        selection.y++;
    }
    return selection;
}

static MELBoolean isCellPartiallyVisible(MELGridView * _Nonnull self, MELIntPoint cell) {
    const MELIntSize cellSize = self->cellSize;
    const MELIntSize cellSpacing = self->cellSpacing;
    const MELInset inset = self->inset;
    const MELPoint camera = self->camera;
    const MELRectangle cameraFrame = (MELRectangle) {
        .origin = camera,
        .size = self->super.frame.size,
    };
    const MELRectangle selectionFrame = (MELRectangle) {
        .origin = {
            .x = cell.x * (cellSize.width + cellSpacing.width) + inset.left,
           .y = cell.y * (cellSize.height + cellSpacing.height) + inset.top,
        },
        .size = {
            .width = cellSize.width,
            .height = cellSize.height,
        },
    };
    return MELRectangleIntersectsWithRectangle(cameraFrame, selectionFrame);
}

static MELBoolean isCellVisible(MELGridView * _Nonnull self, MELIntPoint cell) {
    const MELIntSize cellSize = self->cellSize;
    const MELIntSize cellSpacing = self->cellSpacing;
    const MELInset inset = self->inset;
    const MELPoint camera = self->camera;
    const MELRectangle cameraFrame = (MELRectangle) {
        .origin = camera,
        .size = self->super.frame.size,
    };
    const MELRectangle selectionFrame = (MELRectangle) {
        .origin = {
            .x = cell.x * (cellSize.width + cellSpacing.width) + inset.left,
            .y = cell.y * (cellSize.height + cellSpacing.height) + inset.top,
        },
        .size = {
            .width = cellSize.width,
            .height = cellSize.height,
        },
    };
    return MELRectangleOriginIsTopLeftContainsRectangle(cameraFrame, selectionFrame);
}

static void setSelection(MELGridView * _Nonnull self, LCDSprite * _Nonnull sprite, MELIntPoint selection) {
    const MELIntSize gridSize = self->gridSize;
    const MELIntPoint oldSelection = self->selection;
    const unsigned int oldIndex = oldSelection.y * gridSize.width + oldSelection.x;
    const unsigned int index = selection.y * gridSize.width + selection.x;
    LCDBitmapRefList images = self->images;
    // NOTE: Si les images sont nulles, cela signifie que les cellules ne sont pas visibles. Elles seront dessinées plus tard.
    MELBoolean mustDraw = false;
    if (images.memory[oldIndex]) {
        self->drawCell(self, images.memory[oldIndex], oldSelection.x, oldSelection.y, false);
        mustDraw = true;
    }
    if (images.memory[index]) {
        self->drawCell(self, images.memory[index], selection.x, selection.y, true);
        mustDraw = true;
    }
    self->selection = selection;
    if (mustDraw) {
        draw(self, playdate->sprite->getImage(sprite));
        playdate->sprite->markDirty(sprite);
    }
}

static void updatePosition(MELGridView * _Nonnull self, LCDSprite * _Nonnull sprite) {
    const MELSpritePositionFixed fixed = self->super.fixed;
    const MELPoint origin = self->super.frame.origin;
    const float x = (fixed & MELSpritePositionFixedX) ? origin.x : origin.x - camera.frame.origin.x;
    const float y = (fixed & MELSpritePositionFixedY) ? origin.y : origin.y - camera.frame.origin.y;
    playdate->sprite->moveTo(sprite, x, y);
}

static void update(LCDSprite * _Nonnull sprite) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    updatePosition(self, sprite);

    if (!self->drawWasDone && playdate->sprite->isVisible(sprite)) {
        draw(self, playdate->sprite->getImage(sprite));
        self->drawWasDone = true;
    }

    if (self->disableInputs) {
        return;
    }

    PDButtons pressed, pressing;
    playdate->system->getButtonState(&pressing, &pressed, NULL);

    const PDButtons didScroll = self->didScroll;
    if (didScroll && didScroll != pressing) {
        self->didScroll = 0;
    } else if (didScroll) {
        self->time += DELTA;
        const float duration = 0.25f;
        if (self->time >= duration) {
            self->time = 0.0f;
            const MELIntPoint oldSelection = self->selection;
            const MELIntPoint selection = nextSelection(self, didScroll);
            if (!MELIntPointEquals(selection, oldSelection)) {
                if (isCellVisible(self, selection)) {
                    self->cameraDistance = MELPointZero;
                } else if (self->enableScrolling) {
                    const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
                    MELPoint cameraDistance = (MELPoint) {
                        .x = (selection.x - oldSelection.x) * cellSize.width,
                        .y = (selection.y - oldSelection.y) * cellSize.height,
                    };
                    self->cameraDistance = cameraDistance;
                }
                self->cameraFrom = self->camera;
                setSelection(self, sprite, selection);
                playdate->sprite->setUpdateFunction(sprite, updateScrollRepeat);
                return;
            }
        }
    }

    const MELIntPoint oldSelection = self->selection;
    const MELIntPoint selection = nextSelection(self, pressed);

    if (!MELIntPointEquals(oldSelection, selection)) {
        setSelection(self, sprite, selection);

        self->time = 0.0f;
        self->didScroll = pressed;
        const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
        MELPoint cameraDistance = (MELPoint) {
            .x = (selection.x - oldSelection.x) * cellSize.width,
            .y = (selection.y - oldSelection.y) * cellSize.height,
        };
        self->cameraDistance = cameraDistance;
        self->totalDistance = sqrtf(cameraDistance.x * cameraDistance.x + cameraDistance.y * cameraDistance.y);
        if (self->enableScrolling && !isCellVisible(self, selection)) {
            self->cameraFrom = self->camera;
            playdate->sprite->setUpdateFunction(sprite, updateScrolling);
        }
    }
}

static void updateScrolling(LCDSprite * _Nonnull sprite) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    updatePosition(self, sprite);

    const float duration = self->totalDistance / self->scrollingSpeed;
    if (self->time < duration) {
        float newTime;
        newTime = self->time += DELTA;
        const float progress = MELEaseInOut(0.0f, duration, newTime);

        self->camera = MELPointAdd(self->cameraFrom, MELPointMultiplyByValue(self->cameraDistance, progress));
        draw(self, playdate->sprite->getImage(sprite));
        return;
    }
    self->time = 0.0f;
    playdate->sprite->setUpdateFunction(sprite, update);
}

static void updateScrollRepeat(LCDSprite * _Nonnull sprite) {
    MELGridView *self = playdate->sprite->getUserdata(sprite);
    updatePosition(self, sprite);

    if (self->disableInputs) {
        self->didScroll = 0;
        playdate->sprite->setUpdateFunction(sprite, updateScrolling);
        return;
    }

    PDButtons pressing;
    playdate->system->getButtonState(&pressing, NULL, NULL);

    const PDButtons didScroll = self->didScroll;
    if (didScroll != pressing) {
        self->didScroll = 0;
        playdate->sprite->setUpdateFunction(sprite, updateScrolling);
        return;
    }

    const float duration = self->totalDistance / self->scrollingSpeed;
    if (self->time < duration) {
        float newTime;
        newTime = self->time += DELTA;
        const float progress = MELEaseInOut(0.0f, duration, newTime);

        self->camera = MELPointAdd(self->cameraFrom, MELPointMultiplyByValue(self->cameraDistance, progress));
        draw(self, playdate->sprite->getImage(sprite));
        return;
    }
    self->time = 0.0f;

    const MELIntPoint oldSelection = self->selection;
    const MELIntPoint selection = nextSelection(self, didScroll);

    if (!MELIntPointEquals(oldSelection, selection)) {
        setSelection(self, sprite, selection);
        self->cameraFrom = self->camera;
        // Calcul la distance que la caméra devra parcourir.
        // La distance totale est inchangée. Elle est utilisée pour calculer le temps de transition.
        if (self->enableScrolling && !isCellVisible(self, selection)) {
            const MELIntSize cellSize = MELIntSizeAdd(self->cellSize, self->cellSpacing);
            MELPoint cameraDistance = (MELPoint) {
                .x = (selection.x - oldSelection.x) * cellSize.width,
                .y = (selection.y - oldSelection.y) * cellSize.height,
            };
            self->cameraDistance = cameraDistance;
        } else {
            // Pas besoin de déplacer la caméra car la cellule est déjà visible.
            self->cameraDistance = MELPointZero;
        }
    } else {
        // Si pas de scrolling, retour à la fonction normale.
        self->didScroll = 0;
        playdate->sprite->setUpdateFunction(sprite, update);
    }
}
