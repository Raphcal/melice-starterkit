//
//  language.c
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#include "language.h"

#include "eventbus.h"
#include "melstring.h"

MELListImplement(MELLocalizedString);

MELLanguage currentLanguage = MELLanguageEnglish;
extern LCDFont * _Nullable currentFont;
extern LCDFont * _Nullable smallFont;

const char * _Nonnull MELLocalizedStringGetText(MELLocalizedString string) {
    char *text = string.text[currentLanguage];
    if (text) {
        return text;
    }
    for (int index = 0; index < MELLanguageCount; index++) {
        char *text = string.text[index];
        if (text) {
            return text;
        }
    }
    return kEmptyString;
}

const char * _Nonnull MELLocalizedStringGetTextForLanguage(MELLocalizedString string, MELLanguage language) {
    return string.text[language];
}
