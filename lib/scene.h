//
//  scene.h
//  Roll
//
//  Created by Raphaël Calabro on 07/11/2022.
//

#ifndef scene_h
#define scene_h

#include "melstd.h"

#include "outputstream.h"
#include "lcdspriteref.h"

#include "../src/classes.h"
#include "../gen/spritenames.h"

typedef struct melscene MELScene;

typedef enum {
    SceneTypeTitle,
    SceneTypeGame,
    SceneTypeScore,
    SceneTypeOnlineMenu,
    SceneTypeFade,
} SceneType;

typedef struct melscene {
    SceneType type;
    void (* _Nonnull init)(MELScene * _Nonnull self);
    void (* _Nonnull dealloc)(MELScene * _Nonnull self);
    int (* _Nonnull update)(void * _Nonnull self);
    void (* _Nullable beforeQuit)(MELScene * _Nonnull self);
    LCDSpriteRefList sprites;
} MELScene;

typedef struct melfade {
    MELScene super;
    MELScene * _Nullable oldScene;
    MELScene * _Nullable nextScene;
} MELFade;

extern MELScene * _Nullable currentScene;

/**
 * Libère la scène actuelle et charge la scène donnée pour en faire la scène courante.
 *
 * @note `MELSceneMakeCurrent` désalloue la scène actuelle. Il faut donc généralement faire un return juste après l'appel à cette fonction.
 */
void MELSceneMakeCurrent(MELScene * _Nonnull self);
void MELSceneAddOrRemoveBackToTitleMenuItem(void);
LCDSprite * _Nullable MELSceneFindSpriteByName(SpriteName spriteName);
LCDSprite * _Nullable MELSceneFindSpriteByClassName(SpriteClassName className);
LCDSprite * _Nullable MELSceneFindSpriteByTag(const uint8_t tag);

void MELSceneFadeTo(MELScene * _Nonnull nextScene, MELScene * _Nonnull (* _Nonnull fadeConstructor)(MELScene * _Nonnull oldScene, MELScene * _Nonnull nextScene));

#endif /* scene_h */
