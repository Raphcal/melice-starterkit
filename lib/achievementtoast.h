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

#define ZINDEX_TOAST 100

extern float MELAchievementToastAdjustLeft;
extern float MELAchievementToastAdjustWidth;

LCDSprite * _Nullable MELAchievementToastConstructor(MELAchievement achievement);

#endif /* achievementtoast_h */
