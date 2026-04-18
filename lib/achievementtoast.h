//
//  achievementtoast.h
//  PokerPokerMagic
//
//  Created by Raphaël Calabro on 05/06/2025.
//

#ifndef achievementtoast_h
#define achievementtoast_h

#include "melstd.h"

#include "achievements.h"
#include "rectangle.h"

#define ZINDEX_TOAST 100

/// Ajustement du centre et de la taille de la popup des succès.
/// `MELAchievementToastAdjust.origin` sera ajouté au centre de la popup pour la déplacer.
/// `MELAchievementToastAdjust.size` sera ajouté à la taille de la popup pour la redimensionner.
extern MELRectangle MELAchievementToastAdjust;

extern const LCDColor kGrey75Pattern;
extern const LCDColor kGrey50Pattern;
extern const LCDColor kGrey25Pattern;
extern const LCDColor kGrey12Pattern;

LCDSprite * _Nullable MELAchievementToastConstructor(MELAchievement achievement);
LCDColor LCDColorBlackWithOpacity(float opacity);

#endif /* achievementtoast_h */
