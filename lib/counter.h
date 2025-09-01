//
//  counter.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 26/10/2023.
//

#ifndef counter_h
#define counter_h

#include "melstd.h"

#include "point.h"
#include "alignment.h"

/**
 * Créé un compteur. La méthode `update` par défaut ne prend pas en compte la caméra.
 *
 * @param origin Point où aligner le texte.
 * @param alignment Alignement par rapport à l'origine.
 * @param font Police à utiliser pour afficher les chiffres.
 * @param value Valeur initiale à afficher.
 */
LCDSprite * _Nonnull MELCounterConstructor(const MELPoint origin, const MELHorizontalAlignment alignment, LCDFont * _Nonnull font, const int value);

/**
 * Modifie la valeur affichée.
 *
 * @param sprite Instance de `MELCounter`.
 * @param value Nombre à afficher.
 */
void MELCounterSetValue(LCDSprite * _Nullable sprite, const int value);

void MELCounterUpdateAnchorWithCurrentOrigin(LCDSprite * _Nonnull sprite);

#endif /* counter_h */
