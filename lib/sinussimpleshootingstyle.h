//
//  sinussimpleshootingstyle.h
//  ColdBird
//
//  Created by Raphaël Calabro on 29/01/2025.
//

#ifndef sinussimpleshootingstyle_h
#define sinussimpleshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const MELShootingStyleClass * _Nonnull SinusShootingStyleGetClass(void);

void SinusShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition);

#endif /* sinussimpleshootingstyle_h */
