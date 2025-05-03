//
//  keyboard.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 23/06/2023.
//

#ifndef keyboard_h
#define keyboard_h

#include "melstd.h"

#include "rectangle.h"
#include "melstring.h"

typedef enum {
    kCapitalizationNormal,
    kCapitalizationWords,
    kCapitalizationSentences,
} MELKeyboardCapitalization;

typedef enum {
    kAnimationTypeNone,
    kAnimationTypeKeyboardShow,
    kAnimationTypeKeyboardHide,
    kAnimationTypeSelectionUp,
    kAnimationTypeSelectionDown,
} MELKeyboardAnimationType;

typedef enum {
    kColumnSymbols,
    kColumnUpper,
    kColumnLower,
    kColumnMenu,
} MELKeyboardColumn;
#define kColumnCount 4

typedef enum {
    kMenuOptionSpace,
    kMenuOptionOK,
    kMenuOptionDelete,
    kMenuOptionCancel,
} MELKeyboardMenuOption;

typedef enum {
    kJiggleLeft = -1,
    kJiggleRight = 1
} MELKeyboardJiggleDirection;

typedef void MELKeyboardCallback(void * _Nullable userdata);
typedef void MELKeyboardWillHideCallback(MELBoolean okButtonPressed, void * _Nullable userdata);

typedef struct melkeyboard {
    MELCharList text;
    MELCharList originalText;
    MELBoolean okButtonPressed;
    float degreesSinceClick;

    MELKeyboardCapitalization capitalizationBehavior;

    MELKeyboardColumn selectedColumn;
    MELKeyboardColumn lastTypedColumn;
    int8_t selectionIndexes[kColumnCount];

    MELBoolean isVisible;
    MELBoolean justOpened;

    MELKeyboardAnimationType currentAnimationType;
    unsigned int animationDuration;
    unsigned int animationStartTime;

    int8_t frameRateAdjustedScrollRepeatDelay;

    int8_t columnJiggle;
    int8_t rowJiggle;
    int8_t rowShift;

    int8_t selectionY;
    MELRectangle frame;
    MELRectangle selectedCharacterRect;

    MELKeyboardCallback * _Nullable textChangedCallback;
    MELKeyboardWillHideCallback * _Nullable willHideCallback;
    MELKeyboardCallback * _Nullable didShowCallback;
    MELKeyboardCallback * _Nullable didHideCallback;
    MELKeyboardCallback * _Nullable animatingCallback;

    void * _Nullable textChangedCallbackUserdata;
    void * _Nullable willHideCallbackUserdata;
    void * _Nullable didShowCallbackUserdata;
    void * _Nullable didHideCallbackUserdata;
    void * _Nullable animatingCallbackUserdata;

    MELBoolean scrollingVertically;
    int8_t scrollRepeatDelay;

    int8_t keyRepeatDelay;

    uint8_t maximumLength;

    unsigned int lastKeyEnteredTime;

    // Animation Variables
    float selectionYOffset;
    float selectionStartY;

    // Sounds
    SamplePlayer * _Nullable samplePlayer;
    AudioSample * _Nullable columnNextSound;
    AudioSample * _Nullable columnPreviousSound;
    AudioSample * _Nullable rowSound;
    AudioSample * _Nullable bumpSound;
    AudioSample * _Nullable keySound;
} MELKeyboard;

extern const float kKeyboardWidth;

MELKeyboard * _Nonnull MELKeyboardNew(void);
void MELKeyboardFree(MELKeyboard * _Nonnull self);

void MELKeyboardShow(MELKeyboard * _Nonnull self, const char * _Nullable newText, const unsigned int newTextLength);
void MELKeyboardHide(MELKeyboard * _Nonnull self);
void MELKeyboardGetText(MELKeyboard * _Nonnull self, char * _Nonnull * _Nullable text, unsigned int * _Nullable count);
void MELKeyboardFillBufferWithText(MELKeyboard * _Nonnull self, char * _Nonnull buffer, int length);
float MELKeyboardGetAnimationProgress(MELKeyboard * _Nonnull self);

void MELKeyboardSetTextChangedCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata);
void MELKeyboardSetWillHideCallback(MELKeyboard * _Nonnull self, MELKeyboardWillHideCallback * _Nullable callback, void * _Nullable userdata);
void MELKeyboardSetDidShowCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata);
void MELKeyboardSetDidHideCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata);
void MELKeyboardSetAnimatingCallback(MELKeyboard * _Nonnull self, MELKeyboardCallback * _Nullable callback, void * _Nullable userdata);

#endif /* keyboard_h */
