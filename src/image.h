//
//  image.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 16/07/2023.
//

#ifndef image_h
#define image_h

#include "common.h"
#include "../lib/melice.h"

const MELSpriteClass * _Nonnull ImageGetClass(void);

/**
 * Créé une nouvelle image et la positionne au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param image Image à afficher dans un sprite.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructor(MELPoint origin, LCDBitmap * _Nonnull image);

LCDSprite * _Nonnull ImageConstructorWithAlignment(LCDBitmap * _Nonnull image, MELPoint origin, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);

/**
 * Charge et affiche une nouvelle image. Elle sera positionnée au point central donné.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param origin Centre de l'image.
 * @param path Chemin de l'image à charger et à afficher.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructorWithPath(MELPoint origin, const char * _Nonnull path);

/**
 * Charge et affiche l'image <code>imageIndex</code> de la palette du sprite <code>spriteName</code>.
 * La palette est libérée avec le sprite.
 *
 * @param spriteName Nom du sprite.
 * @param imageIndex Numéro de l'image à charger.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructorWithSpritePalette(MELPoint origin, SpriteName spriteName, int imageIndex);

/**
 * Créé une nouvelle image opaque avec la couleur donnée de la taille `frame.size`.
 *
 * @discussion Ajoute le sprite à la liste des sprites de la scène courante.
 * @discussion L'emplacement est dans le monde, donc relatif à la position de la caméra.
 * @param frame Centre et taille de l'image.
 * @param color Couleur de l'image.
 * @returns Le sprite créé.
 */
LCDSprite * _Nonnull ImageConstructorWithColor(MELRectangle frame, LCDColor color);

LCDSprite * _Nonnull ImageConstructorWithSelf(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);
LCDSprite * _Nonnull ImageConstructorWithSelfDontPush(MELSprite * _Nonnull self, MELPoint origin, LCDBitmap * _Nonnull image);

void ImageDealloc(LCDSprite * _Nonnull sprite);

void ImageSetStatic(LCDSprite * _Nonnull sprite, MELPoint origin, MELHorizontalAlignment horizontalAlignment, MELVerticalAlignment verticalAlignment);

void ImageSetSpritePaletteImageIndex(LCDSprite * _Nonnull sprite, int imageIndex);

#endif /* image_h */
