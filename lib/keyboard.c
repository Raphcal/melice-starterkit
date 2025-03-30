//
//  keyboard.c
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/06/2023.
//

#include "keyboard.h"

#include <string.h>

#include "crank.h"
#include "melmath.h"
#include "bitmap.h"
#include "scene.h"

static void selectColumn(MELKeyboard * _Nonnull self, MELKeyboardColumn column);
static void moveSelectionDown(MELKeyboard * _Nonnull self, int count, MELBoolean shiftRow);
static void moveSelectionUp(MELKeyboard * _Nonnull self, int count, MELBoolean shiftRow);

#pragma mark - Constants

static LCDFont * _Nullable keyboardFont;
static LCDBitmap * _Nullable menuImageSpace;
static LCDBitmap * _Nullable menuImageOK;
static LCDBitmap * _Nullable menuImageDelete;
static LCDBitmap * _Nullable menuImageCancel;

static const char lowerColumn[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
static const char upperColumn[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
static const char numbersColumn[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', ',', ':', ';', '<', '=', '>', '?', '!', '\'', '"', '#', '$', '%', '&', '(', ')', '*', '+', '-', '/', '|', '\\', '[', ']', '^', '_', '`', '{', '}', '~', '@'};
#define kMenuColumnCount 4
static LCDBitmap * _Nullable menuColumn[kMenuColumnCount];

static const void * _Nonnull columns[] = {numbersColumn, upperColumn, lowerColumn, menuColumn};
static unsigned int columnCounts[] = {sizeof(numbersColumn), sizeof(upperColumn), sizeof(lowerColumn), 4};

#define rightMargin 8.0f
#define standardColumnWidth 36.0f
#define menuColumnWidth 50.0f
#define leftMargin 12.0f

const float kKeyboardWidth = rightMargin + (standardColumnWidth * 3) + menuColumnWidth + leftMargin;

static const float columnWidths[] = {standardColumnWidth, standardColumnWidth, standardColumnWidth, menuColumnWidth};

#define p1 leftMargin
#define p2 (p1 + standardColumnWidth)
#define p3 (p2 + standardColumnWidth)
#define p4 (p3 + standardColumnWidth)

static const float columnPositions[] = {p1, p2, p3, p4};

static const float rowHeight = 38.0f;

// this is used for debouncing, I believe we were getting double letter entry at one point in initial wifi password setup
static const unsigned int minKeyRepeatMilliseconds = 100;


#pragma mark - Keyboard Input Handler

static const float clickDegrees = 360.0f / 15.0f;

static void keyboardInputCranked(MELKeyboard * _Nonnull self, float acceleratedChange) {
    const float degreesSinceClick = self->degreesSinceClick + acceleratedChange;

    if (degreesSinceClick > clickDegrees) {
        const int clickCount = floorf(degreesSinceClick / clickDegrees);
        self->degreesSinceClick = 0.0f;
        moveSelectionDown(self, clickCount, false);
    } else if (degreesSinceClick < -clickDegrees) {
        const int clickCount = ceilf(-degreesSinceClick / clickDegrees);
        self->degreesSinceClick = 0.0f;
        moveSelectionUp(self, clickCount, false);
    } else {
        self->degreesSinceClick = degreesSinceClick;
    }
}


#pragma mark - Easing Functions

static float linearEase(float t, float b, float c, float d) {
    return c * t / d + b;
}

static float outBackEase(float t, float b, float c, float d, float s) {
    if (s == 0) {
        s = 1.70158f;
    }
    t = t / d - 1.0f;
    return floorf(c * (t * t * ((s + 1) * t + s) + 1) + b);
}

#pragma mark - Sounds

static const char kSoundColumnMoveNext[] = "CoreLibs/assets/sfx/selection";
static const char kSoundColumnMovePrevious[] = "CoreLibs/assets/sfx/selection-reverse";
static const char kSoundRowMove[] = "CoreLibs/assets/sfx/click";
static const char kSoundBump[] = "CoreLibs/assets/sfx/denial";
static const char kSoundKeyPress[] = "CoreLibs/assets/sfx/key";

static AudioSample * _Nonnull newSampleOrError(const char * _Nonnull path) {
    AudioSample *sample = playdate->sound->sample->load(path);
    if (sample == NULL) {
        playdate->system->error("Unable to load sample at path: %s", path);
    }
    return sample;
}

static void playSound(SamplePlayer * _Nonnull * _Nullable samplePlayer, AudioSample * _Nonnull * _Nullable sample, const char * _Nonnull path) {
    AudioSample *theSample = *sample;
    if (theSample == NULL) {
        theSample = newSampleOrError(path);
        *sample = theSample;
    }
    SamplePlayer *theSamplePlayer = *samplePlayer;
    if (theSamplePlayer == NULL) {
        theSamplePlayer = playdate->sound->sampleplayer->newPlayer();
        *samplePlayer = theSamplePlayer;
    }
    playdate->sound->sampleplayer->setSample(theSamplePlayer, theSample);
    playdate->sound->sampleplayer->play(theSamplePlayer, 1, 1.0f);
}

static void freeSounds(MELKeyboard * _Nonnull self) {
    if (self->samplePlayer) {
        playdate->sound->sampleplayer->freePlayer(self->samplePlayer);
        self->samplePlayer = NULL;
    }
    if (self->columnNextSound) {
        playdate->sound->sample->freeSample(self->columnNextSound);
        self->columnNextSound = NULL;
    }
    if (self->columnPreviousSound) {
        playdate->sound->sample->freeSample(self->columnPreviousSound);
        self->columnPreviousSound = NULL;
    }
    if (self->rowSound) {
        playdate->sound->sample->freeSample(self->rowSound);
        self->rowSound = NULL;
    }
    if (self->bumpSound) {
        playdate->sound->sample->freeSample(self->bumpSound);
        self->bumpSound = NULL;
    }
    if (self->keySound) {
        playdate->sound->sample->freeSample(self->keySound);
        self->keySound = NULL;
    }
}

#pragma mark - Graphics

/// @brief Draw a rounded rect with a radius of 2.
/// @param rectangle Rectangle to draw.
static void fillRoundRect(MELRectangle rectangle, LCDColor color) {
    if (rectangle.size.width < 4 || rectangle.size.height < 4) {
        playdate->graphics->fillRect(rectangle.origin.x, rectangle.origin.y, rectangle.size.width, rectangle.size.height, color);
        return;
    }
    playdate->graphics->drawLine(rectangle.origin.x + 2, rectangle.origin.y,
                                 rectangle.origin.x + rectangle.size.width - 4, rectangle.origin.y,
                                 1, color);
    playdate->graphics->drawLine(rectangle.origin.x + 1, rectangle.origin.y + 1,
                                 rectangle.origin.x + rectangle.size.width - 2, rectangle.origin.y + 1,
                                 1, color);
    playdate->graphics->fillRect(rectangle.origin.x, rectangle.origin.y + 2, rectangle.size.width, rectangle.size.height - 4, color);
    playdate->graphics->drawLine(rectangle.origin.x + 1, rectangle.origin.y + rectangle.size.height - 2,
                                 rectangle.origin.x + rectangle.size.width - 2, rectangle.origin.y + rectangle.size.height - 2,
                                 1, color);
    playdate->graphics->drawLine(rectangle.origin.x + 2, rectangle.origin.y + rectangle.size.height - 1,
                                 rectangle.origin.x + rectangle.size.width - 4, rectangle.origin.y + rectangle.size.height - 1,
                                 1, color);
}

#pragma mark - Draw

static MELBoolean isShowOrHideAnimation(MELKeyboardAnimationType animationType) {
    return animationType == kAnimationTypeKeyboardShow || animationType == kAnimationTypeKeyboardHide;
}

static void drawKeyboard(MELKeyboard * _Nonnull self) {
    // playdate->graphics->pushContext();
    const struct playdate_graphics gfx = *playdate->graphics;

    const MELKeyboardAnimationType currentAnimationType = self->currentAnimationType;
    const MELBoolean animating = isShowOrHideAnimation(currentAnimationType);

    const MELRectangle frame = self->frame;
    int columnOffsets[kColumnCount];

    if (!animating) {
        for (unsigned int index = 0; index < kColumnCount; index++) {
            columnOffsets[index] = frame.origin.x + columnPositions[index];
        }
    } else {
        const float progress = frame.size.width / kKeyboardWidth;
        for (unsigned int index = 0; index < kColumnCount; index++) {
            columnOffsets[index] = frame.origin.x + (columnPositions[index] * progress);
        }
    }

    const float leftX = frame.origin.x;

    // background
    gfx.setDrawMode(kDrawModeCopy);
    gfx.fillRect(leftX + 2, 0, LCD_COLUMNS - leftX, LCD_ROWS, kColorBlack);
    gfx.fillRect(leftX, 0, 2, LCD_ROWS, kColorWhite);

    // selection
    MELRectangle selectedRect = self->selectedCharacterRect;
    selectedRect.origin.x += leftX;
    switch (currentAnimationType) {
        case kAnimationTypeSelectionUp:
            selectedRect.origin.y += 3;
            break;
        case kAnimationTypeSelectionDown:
            selectedRect.origin.y -= 3;
            break;
        default:
            break;
    }

    const int8_t rowShift = self->rowShift;
    if (rowShift > 0) {
        selectedRect.origin.y += 5;
        self->rowShift = rowShift - 1;
    } else if (rowShift < 0) {
        selectedRect.origin.y -= 5;
        self->rowShift = rowShift + 1;
    }

    const int8_t rowJiggle = self->rowJiggle;
    if (rowJiggle > 0) {
        selectedRect.origin.y -= 3;
        selectedRect.size.height += 2;
        self->rowJiggle = rowJiggle - 1;
    } else if (rowJiggle < 0) {
        selectedRect.origin.y += 1;
        selectedRect.size.height += 2;
        self->rowJiggle = rowJiggle + 1;
    }

    const int8_t columnJiggle = self->columnJiggle;
    if (columnJiggle > 0) {
        selectedRect.origin.x += 1;
        selectedRect.size.width += 2;
        self->columnJiggle = columnJiggle - 1;
    } else if (columnJiggle < 0) {
        selectedRect.origin.x -= 3;
        selectedRect.size.width += 2;
        self->columnJiggle = columnJiggle + 1;
    }


    if (!animating) {
        fillRoundRect(selectedRect, kColorWhite);
    }


    gfx.setDrawMode(kDrawModeNXOR);    


    // menu column
    const MELKeyboardColumn selectedColumn = self->selectedColumn;
    const uint8_t selectedMenuIndex = self->selectionIndexes[kColumnMenu];
    const float w = columnWidths[kColumnMenu];
    const float y = self->selectionY - (selectedMenuIndex * rowHeight) + rowHeight;
    const float x = columnOffsets[kColumnMenu];
    float yOffset = 0;
    if (selectedColumn == kColumnMenu) {
        yOffset = self->selectionYOffset;
    }

    const float cx = x + menuColumnWidth / 2;
    float cy = y - rowHeight/2 + yOffset;

    if (animating) {
        gfx.setDrawMode(kDrawModeCopy);
        gfx.fillRect(x, 0, w, LCD_ROWS, kColorBlack);
        gfx.setDrawMode(kDrawModeNXOR);
        
        if (selectedColumn == kColumnMenu) {
            selectedRect.origin.x = x;
            fillRoundRect(selectedRect, kColorWhite);
        }
    }

    for (unsigned int index = 0; index < kMenuColumnCount; index++) {
        LCDBitmap *glyphImage = menuColumn[index];
        int width, height;
        gfx.getBitmapData(glyphImage, &width, &height, NULL, NULL, NULL);
        gfx.drawBitmap(glyphImage, cx - width / 2, cy - height / 2, kBitmapUnflipped);
        cy += rowHeight;
    }
    
    // letter/symbol columns
    const int fontHeight = playdate->graphics->getFontHeight(keyboardFont);

    playdate->graphics->setFont(keyboardFont);
    for (unsigned int index = 0; index < kColumnMenu; index++) {
        const float w = columnWidths[index];
        int y = self->selectionY;
        int y2 = y;
        const int x = columnOffsets[index];
        float yOffset = 0;

        if (index == selectedColumn
            || (selectedColumn == kColumnLower && index == kColumnUpper)
            || (selectedColumn == kColumnUpper && index == kColumnLower)) {
            // while scrolling vertically, don't offset, instead center letters on selection rect - easier to read and looks better
            if (!self->scrollingVertically) {
                yOffset = self->selectionYOffset;
            }
        }
        
        if (animating) {
            gfx.setDrawMode(kDrawModeCopy);
            gfx.fillRect(x, 0, w, LCD_ROWS, kColorBlack);
            gfx.setDrawMode(kDrawModeNXOR);
            
            if (index == selectedColumn) {
                selectedRect.origin.x = x;
                fillRoundRect(selectedRect, kColorWhite);
            }
        }

        const char *charColumn = columns[index];
        const int selectedIndex = self->selectionIndexes[index];
        char glyph = charColumn[selectedIndex];
        int cx = x;
        int cy = y + 4 + yOffset;

        playdate->graphics->drawText(&glyph, 1, kASCIIEncoding, cx, cy);

        // letters above
        int j = 0;
        while (y2 + rowHeight + yOffset > fontHeight) {
            j += 1;
            y2 -= rowHeight;
            cy = y2 + 4 + yOffset;

            glyph = charColumn[(selectedIndex - j + columnCounts[index]) % (columnCounts[index])];
            playdate->graphics->drawText(&glyph, 1, kASCIIEncoding, cx, cy);
        }

        // letters below
        j = 0;
        while (y + rowHeight + yOffset < LCD_ROWS) {
            j += 1;
            y += rowHeight;
            cy = y + 4 + yOffset;
            
            glyph = charColumn[(selectedIndex + j) % (columnCounts[index])];
            playdate->graphics->drawText(&glyph, 1, kASCIIEncoding, cx, cy);
            
        }
    }
}

#pragma mark - Menu Commands

static void hideKeyboard(MELKeyboard * _Nonnull self, MELBoolean okPressed) {
    self->okButtonPressed = okPressed;
    MELKeyboardHide(self);

    // free up memory
    freeSounds(self);
    MELCharListDeinit(&self->originalText);
}


static void deleteAction(MELKeyboard * _Nonnull self) {
    if (self->text.count == 0) {
        return;
    }
    self->text.count = self->text.count - 1;
    if (self->textChangedCallback) {
        self->textChangedCallback(self->textChangedCallbackUserdata);
    }
}


static void cancelAction(MELKeyboard * _Nonnull self) {
    memcpy(self->text.memory, self->originalText.memory, sizeof(char) * self->originalText.count);
    self->text.count = self->originalText.count;

    if (self->textChangedCallback) {
        self->textChangedCallback(self->textChangedCallbackUserdata);
    }

    hideKeyboard(self, false);
}


static void addLetter(MELKeyboard * _Nonnull self, char newLetter) {
    const char lastLetter = self->text.count > 0
        ? self->text.memory[self->text.count - 1]
        : '_';
    MELCharListPush(&self->text, newLetter);
    if (self->textChangedCallback) {
        self->textChangedCallback(self->textChangedCallbackUserdata);
    }

    MELKeyboardCapitalization capitalizationBehavior = self->capitalizationBehavior;
    if ((newLetter == ' ' && capitalizationBehavior == kCapitalizationWords) ||
        (newLetter == ' ' && lastLetter == '.' && capitalizationBehavior == kCapitalizationSentences)) {
        selectColumn(self, kColumnUpper);
    }
}


static void handleMenuCommand(MELKeyboard * _Nonnull self) {
    const int selectedMenuOption = self->selectionIndexes[kColumnMenu];
    switch (selectedMenuOption) {
        case kMenuOptionDelete:
            deleteAction(self);
            break;
        case kMenuOptionOK:
            hideKeyboard(self, true);
            break;
        case kMenuOptionSpace:
            addLetter(self, ' ');
            break;
        case kMenuOptionCancel:
            cancelAction(self);
            break;
        default:
            playdate->system->error("Unsupported menu option: %d", selectedMenuOption);
            break;
    }
}

static void enterKey(MELKeyboard * _Nonnull self) {
    const MELKeyboardColumn selectedColumn = self->selectedColumn;
    if (selectedColumn == kColumnMenu) {
        handleMenuCommand(self);
    } else {
        const char *column = columns[selectedColumn];
        const char newLetter = column[self->selectionIndexes[selectedColumn]];
        addLetter(self, newLetter);
        self->lastTypedColumn = selectedColumn;
    }
    playSound(&self->samplePlayer, &self->keySound, kSoundKeyPress);
}


static void enterNewLetterIfNecessary(MELKeyboard * _Nonnull self) {
    if (!self->isVisible || isShowOrHideAnimation(self->currentAnimationType)) {
        return;
    }
    
    const unsigned int currentMillis = playdate->system->getCurrentTimeMilliseconds();
    
    PDButtons pressing;
    PDButtons justPressed;
    playdate->system->getButtonState(&pressing, &justPressed, NULL);
    if ((justPressed & kButtonA) && (currentMillis > self->lastKeyEnteredTime + minKeyRepeatMilliseconds)) {
        enterKey(self);
        self->lastKeyEnteredTime = currentMillis;
        
        const float initialKeyRepeatSeconds = 0.3f;
        self->keyRepeatDelay = floorf(initialKeyRepeatSeconds * REFRESH_RATE);
    }
    else if (pressing & kButtonA) {
        if (self->keyRepeatDelay <= 0) {
            enterKey(self);
            const float keyRepeatSeconds = 0.1f; // the following repeat delays should be shorter
            self->keyRepeatDelay = floorf(keyRepeatSeconds * REFRESH_RATE);
        } else {
            self->keyRepeatDelay--;
        }
    }
}

#pragma mark - Animations

static void updateAnimation(MELKeyboard * _Nonnull self) {
    const unsigned int animationTime = playdate->system->getCurrentTimeMilliseconds() - self->animationStartTime;

    if (animationTime >= self->animationDuration) {
        // animation ended
        switch (self->currentAnimationType) {
            case kAnimationTypeKeyboardShow:
                self->frame.origin.x = LCD_COLUMNS - kKeyboardWidth;
                if (self->didShowCallback) {
                    self->didShowCallback(self->didShowCallbackUserdata);
                }
                break;
            case kAnimationTypeKeyboardHide:
                self->frame.origin.x = LCD_COLUMNS;
                self->isVisible = false;
                // reset main update function
                playdate->system->setUpdateCallback(currentScene->update, currentScene);
                if (self->didHideCallback) {
                    self->didHideCallback(self->didShowCallbackUserdata);
                }
                self->text.count = 0;
                break;
            case kAnimationTypeSelectionUp:
            case kAnimationTypeSelectionDown:
                self->selectionYOffset = 0;
                break;
            default:
                // Nothing to do.
                break;
        }
        self->currentAnimationType = kAnimationTypeNone;
    }
    else {
        // see what type of animation we are running, and continue it
        switch (self->currentAnimationType) {
            case kAnimationTypeKeyboardShow:
                self->frame.origin.x = outBackEase(animationTime, LCD_COLUMNS, -kKeyboardWidth, self->animationDuration, 1);
                self->frame.size.width = LCD_COLUMNS - self->frame.origin.x;
                break;
            case kAnimationTypeKeyboardHide:
                self->frame.origin.x = outBackEase(animationTime, LCD_COLUMNS - kKeyboardWidth, kKeyboardWidth, self->animationDuration, 1);
                self->frame.size.width = LCD_COLUMNS - self->frame.origin.x;
                break;
            case kAnimationTypeSelectionUp:
            case kAnimationTypeSelectionDown:
                self->selectionYOffset = linearEase(animationTime, self->selectionStartY, -self->selectionStartY, self->animationDuration);
                break;
            default:
                // Nothing to do.
                break;
        }
    }
}


static void startAnimation(MELKeyboard * _Nonnull self, MELKeyboardAnimationType animationType, unsigned int duration) {
    // finish the last animation before starting this one
    if (self->currentAnimationType != kAnimationTypeNone) {
        self->animationDuration = playdate->system->getCurrentTimeMilliseconds() - self->animationStartTime;
        updateAnimation(self);
    }
    
    self->animationDuration = duration;
    self->currentAnimationType = animationType;
    self->animationStartTime = playdate->system->getCurrentTimeMilliseconds();
}

#pragma mark - Selection

static const unsigned int scrollAnimationDuration = 150;

static void moveSelectionUp(MELKeyboard * _Nonnull self, int count, MELBoolean shiftRow) {
    if (isShowOrHideAnimation(self->currentAnimationType)) {
        return;
    }

    const MELKeyboardColumn selectedColumn = self->selectedColumn;
    if (selectedColumn == kColumnMenu) {
        count = 1;
    }

    int8_t *selectionIndexes = self->selectionIndexes;
    if (selectedColumn == kColumnMenu && selectionIndexes[kColumnMenu] == 0) {
        if (REFRESH_RATE > 30) {
            self->rowJiggle = 2;
        } else {
            self->rowJiggle = 1;
        }
        playSound(&self->samplePlayer, &self->bumpSound, kSoundBump);
        return;
    }

    // moving the selection up means moving the letters down. Set an offset that goes from position of the old current letter and animates to zero

    selectionIndexes[selectedColumn] = (selectionIndexes[selectedColumn] - count + columnCounts[selectedColumn]) % columnCounts[selectedColumn];

    // move upper and lower alphabets together
    if (selectedColumn == kColumnLower) {
        selectionIndexes[kColumnUpper] = selectionIndexes[kColumnLower];
    } else if (selectedColumn == kColumnUpper) {
        selectionIndexes[kColumnLower] = selectionIndexes[kColumnUpper];
    }

    self->selectionYOffset -= (rowHeight * count);
    self->selectionStartY = self->selectionYOffset;

    startAnimation(self, kAnimationTypeSelectionUp, scrollAnimationDuration);
    // let the animation think it's already been going on for a frame
    self->animationStartTime = playdate->system->getCurrentTimeMilliseconds() - (1000 / REFRESH_RATE);
    
    if (shiftRow) {
        if (REFRESH_RATE > 30) {
            self->rowShift = -2;
        } else {
            self->rowShift = -1;
        }
    }

    playSound(&self->samplePlayer, &self->rowSound, kSoundRowMove);
}


static void moveSelectionDown(MELKeyboard * _Nonnull self, int count, MELBoolean shiftRow) {
    if (isShowOrHideAnimation(self->currentAnimationType)) {
        return;
    }

    const MELKeyboardColumn selectedColumn = self->selectedColumn;
    if (selectedColumn == kColumnMenu) {
        count = 1;
    }

    int8_t *selectionIndexes = self->selectionIndexes;
    if (selectedColumn == kColumnMenu && selectionIndexes[kColumnMenu] == kMenuColumnCount - 1) {
        if (REFRESH_RATE > 30) {
            self->rowJiggle = 2;
        } else {
            self->rowJiggle = 1;
        }
        playSound(&self->samplePlayer, &self->bumpSound, kSoundBump);
        return;
    }

    selectionIndexes[selectedColumn] = (selectionIndexes[selectedColumn] + count) % columnCounts[selectedColumn];

    // move upper and lower alphabets together
    if (selectedColumn == kColumnLower) {
        selectionIndexes[kColumnUpper] = selectionIndexes[kColumnLower];
    } else if (selectedColumn == kColumnUpper) {
        selectionIndexes[kColumnLower] = selectionIndexes[kColumnUpper];
    }

    self->selectionYOffset += (rowHeight * count);
    self->selectionStartY = self->selectionYOffset;

    startAnimation(self, kAnimationTypeSelectionDown, scrollAnimationDuration);
    // let the animation think it's already been going on for a frame
    self->animationStartTime = playdate->system->getCurrentTimeMilliseconds() - (1000 / REFRESH_RATE);

    if (shiftRow) {
        if (REFRESH_RATE > 30) {
            self->rowShift = 2;
        } else {
            self->rowShift = 1;
        }
    }

    playSound(&self->samplePlayer, &self->rowSound, kSoundRowMove);
}


static void jiggleColumn(MELKeyboard * _Nonnull self, MELKeyboardJiggleDirection jiggleDirection) {
    const int numFrames = REFRESH_RATE > 30 ? 2 : 1;
    
    self->columnJiggle = jiggleDirection * numFrames;
}

static void selectColumn(MELKeyboard * _Nonnull self, MELKeyboardColumn column) {
    if (isShowOrHideAnimation(self->currentAnimationType)) {
        return;
    }

    const MELKeyboardColumn selectedColumn = self->selectedColumn;
    if (selectedColumn > column) {
        jiggleColumn(self, kJiggleLeft);
    } else {
        jiggleColumn(self, kJiggleRight);
    }

	if (column > selectedColumn) {
		playSound(&self->samplePlayer, &self->columnNextSound, kSoundColumnMoveNext);
    } else {
		playSound(&self->samplePlayer, &self->columnPreviousSound, kSoundColumnMovePrevious);
    }

	self->selectedColumn = column;

    MELRectangle selectedCharacterRect = self->selectedCharacterRect;
	selectedCharacterRect.origin.x = columnPositions[column];
	selectedCharacterRect.size.width = columnWidths[column];
    self->selectedCharacterRect = selectedCharacterRect;
}


static void selectPreviousColumn(MELKeyboard * _Nonnull self) {
	if (isShowOrHideAnimation(self->currentAnimationType)) {
		return;
    }

    const MELKeyboardColumn selectedColumn = self->selectedColumn = (self->selectedColumn - 1 + kColumnCount) % kColumnCount;
	jiggleColumn(self, kJiggleLeft);
	MELRectangle selectedCharacterRect = self->selectedCharacterRect;
	selectedCharacterRect.origin.x = columnPositions[selectedColumn];
	selectedCharacterRect.size.width = columnWidths[selectedColumn];
    self->selectedCharacterRect = selectedCharacterRect;
	playSound(&self->samplePlayer, &self->columnNextSound, kSoundColumnMoveNext);
}

static void selectNextColumn(MELKeyboard * _Nonnull self) {
	if (isShowOrHideAnimation(self->currentAnimationType)) {
		return;
    }

    const MELKeyboardColumn selectedColumn = self->selectedColumn = (self->selectedColumn + 1) % kColumnCount;
	jiggleColumn(self, kJiggleRight);
	MELRectangle selectedCharacterRect = self->selectedCharacterRect;
	selectedCharacterRect.origin.x = columnPositions[selectedColumn];
	selectedCharacterRect.size.width = columnWidths[selectedColumn];
    self->selectedCharacterRect = selectedCharacterRect;
	playSound(&self->samplePlayer, &self->columnPreviousSound, kSoundColumnMovePrevious);
}


static void checkButtonInputs(MELKeyboard * _Nonnull self) {
    int8_t scrollRepeatDelay = self->scrollRepeatDelay;
    int8_t keyRepeatDelay = self->keyRepeatDelay;

    PDButtons pressing;
    PDButtons justPressed;
    PDButtons justReleased;
    playdate->system->getButtonState(&pressing, &justPressed, &justReleased);

	if (justPressed & kButtonUp) {
		moveSelectionUp(self, 1, true);
		scrollRepeatDelay = self->frameRateAdjustedScrollRepeatDelay;
    }
	else if (pressing & kButtonUp) {
		if (scrollRepeatDelay <= 0) {
			moveSelectionUp(self, 1, true);
			self->scrollingVertically = true;
			if (REFRESH_RATE > 30) {
				scrollRepeatDelay = 1;
            }
		} else {
			scrollRepeatDelay--;
        }
    }
	else if (justReleased & kButtonUp) {
		self->scrollingVertically = false;
    }
	else if (justPressed & kButtonDown) {
		moveSelectionDown(self, 1, true);
		scrollRepeatDelay = self->frameRateAdjustedScrollRepeatDelay;
    }
	else if (pressing & kButtonDown) {
		if (scrollRepeatDelay <= 0) {
			moveSelectionDown(self, 1, true);
			self->scrollingVertically = true;
			if (REFRESH_RATE > 30) {
				scrollRepeatDelay = 1;
            }
        } else {
			scrollRepeatDelay--;
        }
    }
	else if (justReleased & kButtonDown) {
		self->scrollingVertically = false;
    }
	else if (justPressed & kButtonLeft) {
		selectPreviousColumn(self);
    }
	else if (justPressed & kButtonRight) {
		selectNextColumn(self);
    }
	else if (justPressed & kButtonB) {
		playSound(&self->samplePlayer, &self->keySound, kSoundKeyPress);
		deleteAction(self);
		const float initialKeyRepeatSeconds = 0.3f;
		keyRepeatDelay = floorf(initialKeyRepeatSeconds * REFRESH_RATE);
    }
	else if (pressing & kButtonB) {	
		if (keyRepeatDelay <= 0) {
			playSound(&self->samplePlayer, &self->keySound, kSoundKeyPress);
			deleteAction(self);
			const float keyRepeatSeconds = 0.1f;
			keyRepeatDelay = floorf(keyRepeatSeconds * REFRESH_RATE);
        } else {
			keyRepeatDelay--;
		}
	}
    self->scrollRepeatDelay = scrollRepeatDelay;
    self->keyRepeatDelay = keyRepeatDelay;
}

#pragma mark - Update

// override on the main playdate.update function so that we can run our animations without requiring timers
static int keyboardUpdate(void * _Nonnull userdata) {
    MELKeyboard *self = userdata;

    if (self->isVisible) {
        enterNewLetterIfNecessary(self);

        if (self->currentAnimationType != kAnimationTypeNone) {
            updateAnimation(self);

            if (isShowOrHideAnimation(self->currentAnimationType)) {
                if (self->animatingCallback) {
                    self->animatingCallback(self->animatingCallbackUserdata);
                }
            }
        }

        if (!self->justOpened) {
            checkButtonInputs(self);
            const float acceleratedChange = MELCrankGetAcceleratedChange();
            if (acceleratedChange != 0.0f) {
                keyboardInputCranked(self, acceleratedChange);
            }
        } else {
            self->justOpened = false;
        }

        currentScene->update(currentScene);
        drawKeyboard(self);
    }
    return true;
}

#pragma mark - Resources

static void loadFontAndImages(void) {
    if (keyboardFont) {
        return;
    }
    keyboardFont = LCDFontLoadOrError("CoreLibs/assets/keyboard/Roobert-24-Keyboard-Medium");
    menuColumn[0] = menuImageSpace = LCDBitmapLoadOrError("CoreLibs/assets/keyboard/menu-space");
    menuColumn[1] = menuImageOK = LCDBitmapLoadOrError("CoreLibs/assets/keyboard/menu-ok");
    menuColumn[2] = menuImageDelete = LCDBitmapLoadOrError("CoreLibs/assets/keyboard/menu-del");
    menuColumn[3] = menuImageCancel = LCDBitmapLoadOrError("CoreLibs/assets/keyboard/menu-cancel");
}

#pragma mark - Public functions

MELKeyboard * _Nonnull MELKeyboardNew(void) {
    MELKeyboard *self = playdate->system->realloc(NULL, sizeof(MELKeyboard));

    loadFontAndImages();
    const int selectionY = LCD_ROWS / 2 - rowHeight / 2 - 2;
    const MELKeyboardColumn selectedColumn = kColumnUpper;

    *self = (MELKeyboard) {
        .capitalizationBehavior = kCapitalizationNormal,

        .selectedColumn = selectedColumn,
        .lastTypedColumn = selectedColumn,
        .selectionIndexes = {0, 0, 0, 1},

        .isVisible = false,
        .justOpened = true,

        .currentAnimationType = kAnimationTypeNone,

        .frameRateAdjustedScrollRepeatDelay = 6,

        .selectionY = selectionY,

        .frame = {
            .origin = {
                .x = LCD_COLUMNS,
                .y = 0
            },
            .size = {
                .width = 0,
                .height = LCD_ROWS
            }
        },
        .selectedCharacterRect = {
            .origin = {
                .x = columnPositions[selectedColumn],
                .y = selectionY
            },
            .size = {
                .width = columnWidths[selectedColumn],
                .height = rowHeight
            }
        },

        .text = MELCharListEmpty,
    };
    return self;
}

void MELKeyboardFree(MELKeyboard * _Nonnull self) {
    MELCharListDeinit(&self->text);
    MELCharListDeinit(&self->originalText);
    freeSounds(self);
    playdate->system->realloc(self, 0);
}

void MELKeyboardShow(MELKeyboard * _Nonnull self, const char * _Nullable newText, const unsigned int newTextLength) {
    if (currentScene == NULL) {
        playdate->system->error("currentScene must be defined before calling MELKeyboardShow()");
		return;
    }

	if (self->isVisible) {
        return;
    }

	self->justOpened = true;

	self->okButtonPressed = false;
    // scroll the menu row to OK
	self->selectionIndexes[kColumnMenu] = 1;
    // move the selection back to the last row a character was entered from
	self->selectedColumn = self->lastTypedColumn;
	self->selectedCharacterRect.origin.x = columnPositions[self->selectedColumn];
	self->selectedCharacterRect.size.width = columnWidths[self->selectedColumn];

    MELCharListEnsureCapacity(&self->originalText, newTextLength);
    memcpy(self->originalText.memory, newText, newTextLength * sizeof(char));
    self->originalText.count = newTextLength;

    MELCharListEnsureCapacity(&self->text, MELIntMax((newTextLength * 2) + 1, 10));
    memcpy(self->text.memory, newText, newTextLength * sizeof(char));
    self->text.count = newTextLength;

    playdate->system->setUpdateCallback(keyboardUpdate, self);

	if (self->currentAnimationType != kAnimationTypeNone) {
		// force the previous animation to finish
		self->animationStartTime = 0;
		updateAnimation(self);
	}
	
	const float scrollDelaySeconds = 0.18f;
	self->frameRateAdjustedScrollRepeatDelay = floorf(scrollDelaySeconds * REFRESH_RATE);
	
	self->isVisible = true;
	startAnimation(self, kAnimationTypeKeyboardShow, 220);
}

void MELKeyboardHide(MELKeyboard * _Nonnull self) {
	if (self->isVisible && self->currentAnimationType == kAnimationTypeNone) {
		startAnimation(self, kAnimationTypeKeyboardHide, 220);

		if (self->willHideCallback) {
			self->willHideCallback(self->okButtonPressed, self->willHideCallbackUserdata);
		}
	}
}

void MELKeyboardGetText(MELKeyboard * _Nonnull self, char * _Nonnull * _Nullable text, unsigned int * _Nullable count) {
    const unsigned int charCount = self->text.count;
    char *data = playdate->system->realloc(*text, (charCount + 1) * sizeof(char));
    memcpy(data, self->text.memory, charCount * sizeof(char));
    data[charCount] = '\0';
    *text = data;
    if (count != NULL) {
        *count = charCount;
    }
}

float MELKeyboardGetAnimationProgress(MELKeyboard * _Nonnull self) {
    const unsigned int animationTime = playdate->system->getCurrentTimeMilliseconds() - self->animationStartTime;
    return (float) animationTime / self->animationDuration;
}

void MELKeyboardSetTextChangedCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata) {
    self->textChangedCallback = callback;
    self->textChangedCallbackUserdata = userdata;
}

void MELKeyboardSetWillHideCallback(MELKeyboard * _Nonnull self, MELKeyboardWillHideCallback * _Nullable callback, void * _Nullable userdata) {
    self->willHideCallback = callback;
    self->willHideCallbackUserdata = userdata;
}

void MELKeyboardSetDidShowCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata) {
    self->didShowCallback = callback;
    self->didShowCallbackUserdata = userdata;
}

void MELKeyboardSetDidHideCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata) {
    self->didHideCallback = callback;
    self->didHideCallbackUserdata = userdata;
}

void MELKeyboardSetAnimatingCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata) {
    self->animatingCallback = callback;
    self->animatingCallbackUserdata = userdata;
}
