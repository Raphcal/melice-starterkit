//
//  achievementtoast.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/06/2025.
//

#include "achievementtoast.h"

#include "sprite.h"
#include "scene.h"
#include "melmath.h"
#include "bitmap.h"

#define TOAST_SOUND_PATH "sfx/toast_sound"

#define TOAST_WIDTH 320
#define TOAST_HEIGHT 42
#define TOAST_BORDER_RADIUS 8
#define TOAST_MARGIN_BOTTOM 8
#define TOAST_MARGIN 32

#define TOTAL_WIDTH LCD_COLUMNS
#define TOTAL_HEIGHT (TOAST_HEIGHT + TOAST_MARGIN)

static const MELLocalizedString kAchievementUnlocked = (MELLocalizedString) {
    .text = {
        "Achievement Unlocked",
        "Succès déverrouillé",
        "実績解除",
        "Conquista desbloqueada"
    }
};

static const LCDColor kGrey75Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,

    // Mask, 0 = transparent, 1 = visible
    0b01110111,
    0b11011101,
    0b01110111,
    0b11011101,
    0b01110111,
    0b11011101,
    0b01110111,
    0b11011101,
};

static const LCDColor kGrey50Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,

    // Mask, 0 = transparent, 1 = visible
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
    0b10101010,
    0b01010101,
};

static const LCDColor kGrey25Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,

    // Mask, 0 = transparent, 1 = visible
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
    0b10001000,
    0b00100010,
};

static const LCDColor kGrey12Pattern = (uintptr_t) (LCDPattern) {
    // Bitmap, 0 = noir, 1 = blanc
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,

    // Mask, 0 = transparent, 1 = visible
    0b10000000,
    0b00001000,
    0b10000000,
    0b00001000,
    0b10000000,
    0b00001000,
    0b10000000,
    0b00001000,
};

float MELAchievementToastAdjustLeft = 0.0f;
float MELAchievementToastAdjustWidth = 0.0f;

// TODO: Générer un LCDBitmap "Achievement Unlocked!" et un LCDBitmap avec le titre du succès et faire un crop pour afficher le texte pendant l'apparition du toast.
// Utiliser playdate->graphics->setScreenClipRect(int x, int y, int width, int height); pour l'affichage du texte.
// Après apparition, faire scroller le texte vers le haut.

typedef struct {
    MELSprite super;
    MELAchievement achievement;
    SamplePlayer * _Nullable samplePlayer;
    AudioSample * _Nullable toastSound;
    // TODO: Donner en argument
    LCDBitmap * _Nullable icon;
    LCDBitmap * _Nullable achievementUnlocked;
    LCDBitmap * _Nullable achievementName;
    float time;
} MELAchievementToast;

static void dealloc(LCDSprite * _Nonnull sprite);
static void updateAppear(LCDSprite * _Nonnull sprite);
static void init(MELAchievementToast * _Nonnull self, LCDSprite * _Nonnull sprite, MELAchievement achievement);

static MELAchievementList queue = (MELAchievementList) {};

static const MELSpriteClass MELAchievementToastClass = (MELSpriteClass) {
    .destroy = dealloc,
};

LCDSprite * _Nullable MELAchievementToastConstructor(MELAchievement achievement) {
    MELAchievementListPush(&queue, achievement);
    if (queue.count > 1) {
        return NULL;
    }
    MELAchievementToast *self = new(MELAchievementToast);
    *self = (MELAchievementToast) {
        .super = {
            .class = &MELAchievementToastClass,
            .frame = {
                .origin = {
                    .x = TOTAL_WIDTH / 2,
                    .y = TOTAL_HEIGHT / 2
                },
                .size = {
                    .width = TOAST_WIDTH,
                    .height = TOAST_HEIGHT
                }
            }
        },
        .samplePlayer = playdate->sound->sampleplayer->newPlayer(),
        .toastSound = playdate->sound->sample->load(TOAST_SOUND_PATH),
    };

    playdate->sound->sampleplayer->setSample(self->samplePlayer, self->toastSound);
    playdate->sound->sampleplayer->play(self->samplePlayer, 1, 1.0f);

    LCDSprite *sprite = playdate->sprite->newSprite();
    playdate->sprite->setImage(sprite, playdate->graphics->newBitmap(TOTAL_WIDTH, TOTAL_HEIGHT, kColorClear), kBitmapUnflipped);
    playdate->sprite->setUserdata(sprite, self);
    playdate->sprite->setZIndex(sprite, ZINDEX_TOAST);
    playdate->sprite->moveTo(sprite, LCD_COLUMNS / 2, LCD_ROWS - TOAST_HEIGHT / 2 - TOAST_MARGIN_BOTTOM);
    playdate->sprite->addSprite(sprite);

    init(self, sprite, achievement);

    return sprite;
}

static void dealloc(LCDSprite * _Nonnull sprite) {
    MELAchievementToast *self = playdate->sprite->getUserdata(sprite);
    if (self->samplePlayer != NULL) {
        playdate->sound->sampleplayer->freePlayer(self->samplePlayer);
        self->samplePlayer = NULL;
    }
    if (self->toastSound != NULL) {
        playdate->sound->sample->freeSample(self->toastSound);
        self->toastSound = NULL;
    }
    if (self->achievementUnlocked != NULL) {
        playdate->graphics->freeBitmap(self->achievementUnlocked);
        self->achievementUnlocked = NULL;
    }
    if (self->achievementName != NULL) {
        playdate->graphics->freeBitmap(self->achievementName);
        self->achievementName = NULL;
    }
    if (self->icon != NULL) {
        playdate->graphics->freeBitmap(self->icon);
        self->icon = NULL;
    }
    playdate->graphics->freeBitmap(playdate->sprite->getImage(sprite));
    playdate->system->realloc(self, 0);
    playdate->sprite->removeSprite(sprite);
    playdate->sprite->freeSprite(sprite);
    queue.count = 0;
}

static void drawRoundedRect(MELRectangle rectangle, int radius, float lineWidth, LCDColor color) {
    if (lineWidth <= 0.01f) {
        return;
    }
    const MELPoint topLeft = MELRectangleOriginIsCenterGetTopLeft(rectangle);

    int lineWidthAsInt = MELIntMax(lineWidth, 1);

    const struct playdate_graphics *gfx = playdate->graphics;
    if (lineWidth <= 0.12f) {
        color = kGrey12Pattern;
    } else if (lineWidth <= 0.25f) {
        color = kGrey25Pattern;
    } else if (lineWidth <= 0.5f) {
        color = kGrey50Pattern;
    } else if (lineWidth <= 0.75f) {
        color = kGrey75Pattern;
    }
    gfx->drawRoundRect(topLeft.x, topLeft.y, rectangle.size.width, rectangle.size.height, radius, lineWidthAsInt, color);
}

static void fillRoundedRect(MELRectangle rectangle, int radius, LCDColor color) {
    const MELPoint topLeft = MELRectangleOriginIsCenterGetTopLeft(rectangle);

    const struct playdate_graphics *gfx = playdate->graphics;
    gfx->fillRoundRect(topLeft.x, topLeft.y, rectangle.size.width, rectangle.size.height, radius, color);
}

static LCDBitmap * _Nonnull drawText(LCDFont * _Nonnull font, LCDBitmapDrawMode drawMode, const char * _Nonnull text, int length) {
    const struct playdate_graphics *gfx = playdate->graphics;
    const int width = gfx->getTextWidth(font, text, length, kUTF8Encoding, 0);
    const int height = gfx->getFontHeight(font);

    LCDBitmap *bitmap = playdate->graphics->newBitmap(width, height, kColorClear);
    gfx->pushContext(bitmap);
    gfx->setFont(font);
    gfx->setDrawMode(drawMode);
    gfx->setTextLeading(0);
    gfx->drawText(text, length, kUTF8Encoding, 0, 0);
    gfx->setTextLeading(0);
    gfx->popContext();

    return bitmap;
}

static void init(MELAchievementToast * _Nonnull self, LCDSprite * _Nonnull sprite, MELAchievement achievement) {
    self->time = 0.0f;
    self->achievement = achievement;
    if (achievement.icon) {
        self->icon = LCDBitmapLoadOrError(achievement.icon);
    }
    MELRectangle frame = (MELRectangle) {
        .origin = {
            .x = TOTAL_WIDTH / 2 + MELAchievementToastAdjustLeft,
            .y = TOTAL_HEIGHT / 2
        },
        .size = {
            .width = TOAST_WIDTH - MELAchievementToastAdjustWidth,
            .height = TOAST_HEIGHT
        }
    };

    self->super.frame = frame;
    playdate->sound->sampleplayer->play(self->samplePlayer, 1, 1.0f);
    // TODO: Calculer la taille en fonction de la scène
    playdate->sprite->setUpdateFunction(sprite, updateAppear);
}

static void updateDisplayNextAchievement(LCDSprite * _Nonnull sprite) {
    MELAchievementToast *self = playdate->sprite->getUserdata(sprite);
    if (self->achievementUnlocked != NULL) {
        playdate->graphics->freeBitmap(self->achievementUnlocked);
        self->achievementUnlocked = NULL;
    }
    if (self->achievementName != NULL) {
        playdate->graphics->freeBitmap(self->achievementName);
        self->achievementName = NULL;
    }
    if (self->icon != NULL) {
        playdate->graphics->freeBitmap(self->icon);
        self->icon = NULL;
    }
    init(self, sprite, queue.memory[0]);
}

static void updateDisappear(LCDSprite * _Nonnull sprite) {
    MELAchievementToast *self = playdate->sprite->getUserdata(sprite);
    const float kDuration = 0.5f;
    const float time = self->time += DELTA;
    const float progress = 1.0f - MELEaseInBack(0, kDuration, time);

    if (time >= kDuration) {
        self->time = 0.0f;
        MELAchievementListRemove(&queue, 0);
        playdate->sprite->setUpdateFunction(sprite, queue.count == 0
            ? self->super.class->destroy
            : updateDisplayNextAchievement);
    }

    LCDBitmap *image = playdate->sprite->getImage(sprite);

    const struct playdate_graphics *gfx = playdate->graphics;
    gfx->clearBitmap(image, kColorClear);
    gfx->pushContext(image);

    const MELPoint center = self->super.frame.origin;
    MELRectangle rect = (MELRectangle) {
        .origin = center,
        .size = MELSizeMultiplyByValue(self->super.frame.size, progress)
    };
    fillRoundedRect(rect, TOAST_BORDER_RADIUS, kColorBlack);
    drawRoundedRect(rect, TOAST_BORDER_RADIUS, 1, kColorWhite);

    rect.size.width -= 4;
    rect.size.height -= 4;
    drawRoundedRect(rect, TOAST_BORDER_RADIUS, 1, kColorWhite);

    const MELSize clipRectSize = (MELSize) {
        .width = rect.size.width - 8,
        .height = rect.size.height - 4
    };
    if (clipRectSize.width > 0 && clipRectSize.height > 0) {
        const MELPoint topLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMakeWithOriginAndSize(center, clipRectSize));
        gfx->setClipRect(topLeft.x, topLeft.y, clipRectSize.width, clipRectSize.height);
        
        int iconWidth, iconHeight;
        if (self->icon) {
            gfx->getBitmapData(self->icon, &iconWidth, &iconHeight, NULL, NULL, NULL);
            gfx->drawBitmap(self->icon, topLeft.x, TOTAL_HEIGHT / 2 - iconHeight / 2, kBitmapUnflipped);
        } else {
            iconWidth = 0;
        }
        
        int width, height;
        gfx->getBitmapData(self->achievementName, &width, &height, NULL, NULL, NULL);
        const MELPoint textTopLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMake(center.x, center.y, width, height));
        gfx->drawBitmap(self->achievementName, textTopLeft.x + iconWidth / 2, textTopLeft.y, kBitmapUnflipped);
        
        gfx->clearClipRect();
    }
    gfx->popContext();
    playdate->sprite->markDirty(sprite);
}

static void updateWait(LCDSprite * _Nonnull sprite) {
    MELAchievementToast *self = playdate->sprite->getUserdata(sprite);

    if (self->achievementName == NULL) {
        self->achievementName = drawText(currentFont, kDrawModeFillWhite, MELLocalizedStringGetText(self->achievement.name), 99);
    }

    const float kDuration = 3.0f;
    const float oldProgress = MELEaseInOut(0.75f, 1.75f, self->time);
    const float time = self->time += DELTA;

    if (time >= kDuration) {
        self->time = 0.0f;
        playdate->sprite->setUpdateFunction(sprite, updateDisappear);
    }

    const MELSize size = self->super.frame.size;
    const MELSize clipRectSize = (MELSize) {
        .width = size.width - 8 - 4,
        .height = size.height - 4 - 4
    };

    const float textProgress = MELEaseInOut(0.75f, 1.75f, time);
    if (textProgress && textProgress != oldProgress) {
        LCDBitmap *image = playdate->sprite->getImage(sprite);
        const struct playdate_graphics *gfx = playdate->graphics;
        gfx->pushContext(image);

        int iconWidth = 0;
        if (self->icon) {
            gfx->getBitmapData(self->icon, &iconWidth, NULL, NULL, NULL, NULL);
        }

        const MELPoint center = self->super.frame.origin;
        const MELPoint topLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMakeWithOriginAndSize(center, clipRectSize));
        gfx->fillRect(topLeft.x + iconWidth, topLeft.y, clipRectSize.width - iconWidth, clipRectSize.height, kColorBlack);
        gfx->setClipRect(topLeft.x, topLeft.y, clipRectSize.width, clipRectSize.height);

        int width, height;
        gfx->getBitmapData(self->achievementUnlocked, &width, &height, NULL, NULL, NULL);
        const float travelHeight = size.height / 2 + height / 2;
        const MELPoint textTopLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMake(center.x, center.y, width, height));
        gfx->drawBitmap(self->achievementUnlocked, textTopLeft.x + iconWidth / 2, textTopLeft.y - textProgress * travelHeight, kBitmapUnflipped);

        gfx->getBitmapData(self->achievementName, &width, &height, NULL, NULL, NULL);
        const float left = center.x - width / 2 + iconWidth / 2;
        gfx->drawBitmap(self->achievementName, left, textTopLeft.y + (1.0f - textProgress) * travelHeight, kBitmapUnflipped);

        gfx->popContext();
        playdate->sprite->markDirty(sprite);
    }
}

static void updateAppear(LCDSprite * _Nonnull sprite) {
    MELAchievementToast *self = playdate->sprite->getUserdata(sprite);

    if (self->achievementUnlocked == NULL) {
        self->achievementUnlocked = drawText(currentFont, kDrawModeFillWhite, MELLocalizedStringGetText(kAchievementUnlocked), 99);
    }

    const float kDuration = 1.0f;
    const float time = self->time += DELTA;
    const float progress = MELEaseOutBack(0, kDuration, time);
    const float shadowProgress = MELEaseExpo(0.5f, kDuration, time);

    if (time >= kDuration) {
        self->time = 0.0f;
        playdate->sprite->setUpdateFunction(sprite, updateWait);
    }

    LCDBitmap *image = playdate->sprite->getImage(sprite);

    const struct playdate_graphics *gfx = playdate->graphics;
    gfx->clearBitmap(image, kColorClear);
    gfx->pushContext(image);

    const MELPoint center = self->super.frame.origin;
    MELRectangle rect = (MELRectangle) {
        .origin = center,
        .size = MELSizeMultiplyByValue(self->super.frame.size, progress)
    };

    if (shadowProgress && shadowProgress < 1.0f) {
        const MELSize shadowSize = (MELSize) {
            .width = rect.size.width + TOAST_MARGIN / 2 * shadowProgress,
            .height = rect.size.height + TOAST_MARGIN / 2 * shadowProgress
        };
        drawRoundedRect(MELRectangleMakeWithOriginAndSize(center, shadowSize), TOAST_BORDER_RADIUS + TOAST_BORDER_RADIUS * progress, (1.0f - shadowProgress) * 8.0f, kColorWhite);
    }

    fillRoundedRect(rect, TOAST_BORDER_RADIUS, kColorBlack);
    drawRoundedRect(rect, TOAST_BORDER_RADIUS, 1, kColorWhite);
    rect.size.width -= 4;
    rect.size.height -= 4;
    drawRoundedRect(rect, TOAST_BORDER_RADIUS, 1, kColorWhite);

    const float textProgress = MELEaseInOut(0.25f, 0.75f, time);
    const MELSize clipRectSize = (MELSize) {
        .width = rect.size.width - 8,
        .height = rect.size.height - 4
    };
    if (clipRectSize.width > 0 && clipRectSize.height > 0) {
        const MELPoint topLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMakeWithOriginAndSize(center, clipRectSize));
        gfx->setClipRect(topLeft.x, topLeft.y, clipRectSize.width, clipRectSize.height);

        int iconWidth, iconHeight;
        if (self->icon) {
            gfx->getBitmapData(self->icon, &iconWidth, &iconHeight, NULL, NULL, NULL);
            gfx->drawBitmap(self->icon, topLeft.x, TOTAL_HEIGHT / 2 - iconHeight / 2, kBitmapUnflipped);
        } else {
            iconWidth = 0;
        }
        if (textProgress) {
            int width, height;
            gfx->getBitmapData(self->achievementUnlocked, &width, &height, NULL, NULL, NULL);
            const float travelHeight = TOAST_HEIGHT / 2 + height / 2;
            const MELPoint textTopLeft = MELRectangleOriginIsCenterGetTopLeft(MELRectangleMake(center.x + iconWidth / 2, center.y, width, height));
            gfx->drawBitmap(self->achievementUnlocked, textTopLeft.x, textTopLeft.y + (1.0f - textProgress) * travelHeight, kBitmapUnflipped);
        }
        gfx->clearClipRect();
    }

    gfx->popContext();
    playdate->sprite->markDirty(sprite);
}
