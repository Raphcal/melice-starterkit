//
//  subsprite.h
//  Roll
//
//  Created by Raphaël Calabro on 06/05/2022.
//

#ifndef subsprite_h
#define subsprite_h

#include "melstd.h"

#include "sprite.h"
#include "list.h"

typedef struct melsubsprite MELSubSprite;

typedef struct melsubsprite {
    MELSprite super;
    /// Instance de LCDSprite de ce sous-sprite.
    LCDSprite * _Nullable sprite;
    /// Référence vers ce sprite dans la structure du sprite parent.
    /// Permet de passer à `NULL` la référence du parent quand ce subsprite est détruit pour éviter de gérer un subsprite détruit.
    MELSubSprite * _Nullable * _Nullable parentRef;
} MELSubSprite;

typedef MELSubSprite * _Nullable MELSubSpriteRef;

MELListDefine(MELSubSpriteRef);
MELListDefineSaveRef(MELSubSprite);

/**
 * Créé un nouveau sous-sprite pour le parent donné.
 * Le parent n'est utilisé que pour le z-index actuellement. Il n'est pas stocké au niveau du sous-sprite.
 *
 * @param parent Parent du sous-sprite.
 * @param definition Définition à utiliser (en général, la même que le parent).
 * @param animation Animation initiale du sous-sprite.
 */
MELSubSprite * _Nonnull MELSubSpriteAlloc(LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation);

/**
 * Créé un nouveau sous-sprite et l'ajoute à la liste des sprites de la scène courante pour permettre
 * la gestion des collisions.
 *
 * @param parent Parent du sous-sprite.
 * @param definition Définition à utiliser (en général, la même que le parent).
 * @param animation Animation initiale du sous-sprite.
 */
MELSubSprite * _Nonnull MELSubSpriteAllocAndPush(LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation);

void MELSubSpriteInit(MELSubSprite * _Nonnull self, LCDSprite * _Nonnull parent, MELSpriteDefinition * _Nonnull definition, AnimationName animation);

/**
 * Fait la même chose que MELSpriteDealloc mais ne retire pas de la liste des sprites car les MELSubSprite n'y sont généralement pas présent.
 * Permet d'exécuter le LCDSpriteRefListRemove après la désallocation de la scène.
 */
void MELSubSpriteDealloc(LCDSprite * _Nonnull sprite);

void MELSubSpriteSave(MELSubSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
void MELSubSpriteSaveNullable(MELSubSprite * _Nullable sprite, MELOutputStream * _Nonnull outputStream);
MELSubSprite * _Nullable MELSubSpriteLoad(MELInputStream * _Nonnull inputStream);
MELSubSprite * _Nullable MELSubSpriteLoadNullable(MELInputStream * _Nonnull inputStream);

void MELSubSpriteRefDeinit(MELSubSpriteRef * _Nonnull self);

/// Remplace la méthode update du sprite par `MELSpriteDealloc` ou libère la mémoire donnée si le sprite est déjà supprimé.
void MELSubSpriteDeallocFromParent(MELSubSprite * _Nonnull self);

void MELSubSpriteDetachFromParent(MELSprite * _Nonnull self);

#endif /* subsprite_h */
