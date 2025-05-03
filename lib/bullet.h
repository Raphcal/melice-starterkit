//
//  shot.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 09/02/2023.
//

#ifndef bullet_h
#define bullet_h

#include "melstd.h"

#include "shootingstyledefinition.h"
#include "point.h"
#include "sprite.h"

LCDSprite * _Nonnull BulletConstructor(const MELShootingStyleDefinition * _Nonnull definition, MELPoint origin, MELPoint speed);

const MELSpriteClass * _Nonnull BulletGetClass(void);

#endif /* bullet_h */
