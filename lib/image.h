//
//  image.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/07/2023.
//

#ifndef image_h
#define image_h

#include "melstd.h"
#include "sprite.h"
#include "alignment.h"

const MELSpriteClass * _Nonnull MELImageGetClass(void);

/**
 * Créé une nouvelle image et la positionne au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param image Image à afficher dans un sprite.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull MELImageConstructor(MELPoint origin, LCDBitmap * _Nonnull image);

LCDSprite * _Nonnull MELImageConstructorWithAlignment(LCDBitmap * _Nonnull image, MELPoint origin, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);

/**
 * Charge et affiche une nouvelle image. Elle sera positionnée au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param path Chemin de l'image à charger et à afficher.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull MELImageConstructorWithPath(MELPoint origin, const char * _Nonnull path);

/**
 * Charge et affiche l'image <code>imageIndex</code> de la palette du sprite <code>spriteName</code>.
 * La palette est libérée avec le sprite.
 *
 * @param spriteName Nom du sprite.
 * @param imageIndex Numéro de l'image à charger.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull MELImageConstructorWithSpritePalette(MELPoint origin, SpriteName spriteName, int imageIndex);

/**
 * Créé une nouvelle image opaque avec la couleur donnée de la taille `frame.size`.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param frame Centre et taille de l'image.
 * @param color Couleur de l'image.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull MELImageConstructorWithColor(MELRectangle frame, LCDColor color);

LCDSprite * _Nonnull MELImageConstructorWithSelf(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);
LCDSprite * _Nonnull MELImageConstructorWithSelfDontPush(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);

void MELImageDealloc(LCDSprite * _Nonnull sprite);

void MELImageSetStatic(LCDSprite * _Nonnull sprite, MELPoint origin, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);

void MELImageSetSpritePaletteImageIndex(LCDSprite * _Nonnull sprite, int imageIndex);

#endif /* image_h */
