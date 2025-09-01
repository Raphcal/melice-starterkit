//
//  achievements.h
//  Montgolfier
//
//  Created by Raphaël Calabro on 23/05/2025.
//

#ifndef melachievements_h
#define melachievements_h

#include "melstd.h"

#include "language.h"
#include "list.h"

#define EXPORT_ACHIEVEMENT_ICONS 0

typedef struct {
    const char * _Nonnull id;
    MELLocalizedString name;
    MELLocalizedString description;
    MELLocalizedString descriptionLocked;
    const char * _Nullable icon;
    const char * _Nullable iconLocked;
    uint32_t scoreValue;
    int32_t progressMax;
    uint32_t hashcode;
    MELBoolean progressIsPercentage;
    MELBoolean isSecret;
} MELAchievement;

MELListDefine(MELAchievement);

typedef struct {
    const char * _Nullable iconPath;
    const char * _Nullable cardPath;
    MELAchievementList achievements;
} MELAchievementData;

typedef struct {
    uint32_t grantedAt;
    int32_t progress;
} MELAchievementStatus;

typedef MELAchievementStatus * _Nullable MELAchievementStatusRef;
MELListDefine(MELAchievementStatus);

void MELAchievementSaveStatus(void);
void MELAchievementLoadStatus(const MELAchievementData data);

void MELAchievementGrant(unsigned int achievementIndex);
void MELAchievementGrantAt(unsigned int achievementIndex, uint32_t grantedAt);
void MELAchievementRevoke(unsigned int achievementIndex);
void MELAchievementAdvanceTo(unsigned int achievementIndex, int32_t advanceTo);

#endif /* melachievements_h */
