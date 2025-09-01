//
//  language.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 02/01/2024.
//

#ifndef language_h
#define language_h

#include "melstd.h"

#include "list.h"

typedef enum {
    MELLanguageEnglish,
    MELLanguageFrench,
    MELLanguageJapanese,
    MELLanguageBrazilianPortuguese,
} MELLanguage;
#define MELLanguageCount 4

typedef struct {
    char * _Nullable text[MELLanguageCount];
} MELLocalizedString;
MELListDefine(MELLocalizedString);

extern MELLanguage currentLanguage;

/**
 * Récupère la chaîne traduite dans le langage courant. Si la phrase traduite n'existe pas,
 * récupère la phrase dans une autre langue (ordre : anglais → français → japonais).
 *
 * @param string Chaîne à traduire.
 * @returns La chaîne traduite ou une chaîne vide si aucun texte n'est renseigné.
 */
const char * _Nonnull MELLocalizedStringGetText(MELLocalizedString string);

const char * _Nonnull MELLocalizedStringGetTextForLanguage(MELLocalizedString string, MELLanguage language);

#endif /* language_h */
