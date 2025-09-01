//
//  sprite.h
//  Roll
//
//  Created by Raphaël Calabro on 27/08/2022.
//

#ifndef sprite_h
#define sprite_h

#include "melstd.h"

#include "spritedefinition.h"
#include "animation.h"
#include "rectangle.h"
#include "direction.h"
#include "hitbox.h"
#include "spriteinstance.h"
#include "inputstream.h"
#include "outputstream.h"
#include "lcdspriteref.h"

#include "../gen/animationnames.h"
#include "../src/classes.h"

typedef struct mellayer MELLayer;

typedef struct melspriteclass {
    SpriteClassName name;
    void (* _Nonnull destroy)(LCDSprite * _Nonnull self);
    void (* _Nullable update)(LCDSprite * _Nonnull self);
    void (* _Nullable collidesWithPlayer)(LCDSprite * _Nonnull self, LCDSprite * _Nonnull player);

    void (* _Nullable save)(MELSprite * _Nonnull self, MELOutputStream * _Nonnull outputStream);
    MELSprite * _Nullable (* _Nullable load)(MELSpriteDefinition * _Nonnull definition, LCDSprite * _Nonnull sprite, MELInputStream * _Nonnull inputStream);
} MELSpriteClass;

extern const MELSpriteClass MELSpriteClassDefault;

typedef enum {
    MELSpritePositionFixedX = 1,
    MELSpritePositionFixedY = 2,
    MELSpritePositionFixedBoth = 3,
} MELSpritePositionFixed;

typedef struct melsprite {
    const MELSpriteClass * _Nonnull class;
    MELSpriteDefinition definition;
    AnimationName animationName;
    MELAnimationDirection animationDirection;
    MELAnimation * _Nullable animation;
    /// Emplacement et taille du sprite. L'origine est le centre.
    MELRectangle frame;
    MELDirection direction;
    MELHitbox * _Nullable hitbox;

    // Lien avec le SpriteLoader
    MELSpriteInstance * _Nullable instance;
    void * _Nullable userdata;
    MELBoolean autoReleaseUserdata;

    // Permet de fixer la position x ou y par rapport à la caméra.
    MELSpritePositionFixed fixed;
} MELSprite;

/**
 * Initialise le sprite donné avec la classe par défaut.
 *
 * Toutes les valeurs de `self` seront écrasées. Pour définir une classe, il faudra modifier `self->class` après l'appel à cette méthode.
 *
 * @note Aucune méthode de mise à jour n'est définie, vous devez en définir une vous-même sur l'objet `LCDSprite` retourné.
 *
 * @param self Instance de `MELSprite` ou d'un descendant à initialiser.
 * @param definition Pointeur sur la définition du sprite.
 * @param instance Instance du sprite.

 * @return Une instance de `LCDSprite` ayant `self` comme `userdata`.
 */
LCDSprite * _Nonnull MELSpriteInit(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELSpriteInstance * _Nonnull instance);

/**
 * Initialise le sprite donné avec la classe par défaut.
 *
 * Toutes les valeurs de `self` seront écrasées. Pour définir une classe, il faudra modifier `self->class` après l'appel à cette méthode.
 *
 * Aucune méthode de mise à jour n'est définie, vous devez en définir une vous-même sur l'objet `LCDSprite` retourné.
 *
 * @param self Instance de `MELSprite` ou d'un descendant à initialiser.
 * @param definition Pointeur sur la définition du sprite.
 * @param center Centre où placer le sprite.
 * @return Une instance de `LCDSprite` ayant `self` comme `userdata`.
 */
LCDSprite * _Nonnull MELSpriteInitWithCenter(MELSprite * _Nonnull self, MELSpriteDefinition * _Nonnull definition, MELPoint center);

LCDSprite * _Nonnull MELSpriteInitHiddenWithUpdate(MELSprite * _Nonnull self, void (* _Nullable update)(LCDSprite * _Nonnull));

/**
 * Désalloue l'instance de `MELSprite` positionnée en `userdata` du `LCDSprite` donné et supprime le `LCDSprite`.
 *
 * @param sprite Sprite à supprimer et désallouer
 */
void MELSpriteDealloc(LCDSprite * _Nonnull sprite);

void MELSpriteNoopUpdate(LCDSprite * _Nonnull sprite);
void MELSpriteNoopDraw(LCDSprite * _Nonnull sprite, PDRect bounds, PDRect drawrect);

void MELSpriteUpdate(LCDSprite * _Nonnull sprite);
void MELSpriteUpdateDisappearing(LCDSprite * _Nonnull sprite);

void MELSpriteDraw(MELSprite * _Nonnull self, LCDSprite * _Nonnull sprite);

void MELSpriteSave(LCDSprite * _Nonnull sprite, MELOutputStream * _Nonnull outputStream);
LCDSprite * _Nullable MELSpriteLoad(MELInputStream * _Nonnull inputStream);

void MELSpriteSetAnimation(MELSprite * _Nonnull self, AnimationName animationName);
void MELSpriteSetAnimationAndDirection(MELSprite * _Nonnull self, AnimationName animationName, MELAnimationDirection direction);

void MELSpriteSetLeft(MELSprite * _Nonnull self, float left);
void MELSpriteSetRight(MELSprite * _Nonnull self, float right);
void MELSpriteMoveBy(MELSprite * _Nonnull self, MELPoint translation);

void MELSpriteSetPositionFixed(MELSprite * _Nonnull self, MELSpritePositionFixed fixed);

void MELSpriteMakeDisappear(LCDSprite * _Nonnull sprite);

MELBoolean MELSpriteIsLookingToward(MELSprite * _Nonnull self, MELPoint point);

MELRectangle MELSpriteGetFrame(LCDSprite * _Nonnull sprite);
float MELSpriteGetWidth(LCDSprite * _Nonnull sprite);
float MELSpriteGetHeight(LCDSprite * _Nonnull sprite);

MELRectangle LCDSpriteGetFrame(LCDSprite * _Nonnull sprite);
void LCDSpriteMoveBy(LCDSprite * _Nonnull sprite, MELPoint translation);
void LCDSpriteSetClass(LCDSprite * _Nonnull sprite, const MELSpriteClass * _Nonnull class);
void LCDSpriteSetPositionFixed(LCDSprite * _Nonnull sprite, MELSpritePositionFixed fixed);

#endif /* sprite_h */
