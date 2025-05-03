//
//  counter.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/10/2023.
//

#ifndef counter_h
#define counter_h

#include "common.h"
#include "../lib/melice.h"

typedef enum {
    /**
     * Aligne le texte du compteur au centre-gauche.
     */
    kCounterAlignmentLeft,
    /**
     * Aligne le texte du compteur au centre-center.
     */
    kCounterAlignmentCenter,
    /**
     * Aligne le texte du compteur au centre-droit.
     */
    kCounterAlignmentRight,
} CounterAlignment;

/**
 * Créé un compteur. La méthode `update` par défaut ne prend pas en compte la caméra.
 *
 * @param origin Point où aligner le texte.
 * @param alignment Alignement par rapport à l'origine.
 * @param font Police à utiliser pour afficher les chiffres.
 * @param value Valeur initiale à afficher.
 */
LCDSprite * _Nonnull CounterConstructor(const MELPoint origin, const CounterAlignment alignment, LCDFont * _Nonnull font, const int value);

/**
 * Modifie la valeur affichée.
 *
 * @param sprite Instance de `Counter`.
 * @param value Nombre à afficher.
 */
void CounterSetValue(LCDSprite * _Nullable sprite, const int value);

void CountUpdateAnchorWithCurrentOrigin(LCDSprite * _Nonnull sprite);

#endif /* counter_h */
