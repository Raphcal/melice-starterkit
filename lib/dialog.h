//
//  dialog.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 13/07/2024.
//

#ifndef dialog_h
#define dialog_h

#include "melstd.h"

#include "alignment.h"
#include "nineslice.h"
#include "gridview.h"

typedef void (* _Nullable MELDialogGetChoice)(const void * _Nullable userdata, int index, char * _Nonnull buffer, int bufferLength);

typedef struct {
    /**
     * Taille et position de la boîte de dialogue. Utiliser 0 pour calculer automatiquement.
     */
    MELRectangle frame;
    const char * _Nullable message;
    const char * _Nullable * _Nullable choices;
    const void * _Nullable userdata;
    MELDialogGetChoice getChoiceLabel;
    int count;
    int initialSelection;
    MELInset padding;
    MELNineSlice * _Nullable border;
    MELNineSlice * _Nullable selectionBorder;
    LCDFont * _Nullable font;
} MELDialogConfig;

typedef enum {
    MELDialogStateOpening,
    MELDialogStateOpened,
    MELDialogStateClosing,
    MELDialogStateClosed,
} MELDialogState;

typedef struct {
    MELSprite super;
    MELDialogConfig config;
    MELDialogState state;
    MELGridView * _Nullable gridView;
    LCDSprite * _Nullable gridViewSprite;
    MELIntPoint textTopLeft;
    float time;
} MELDialog;

LCDSprite * _Nonnull MELDialogMake(MELDialogConfig config);
unsigned int MELDialogGetSelection(LCDSprite * _Nonnull sprite);
void MELDialogClose(LCDSprite * _Nonnull sprite);

#endif /* dialog_h */
