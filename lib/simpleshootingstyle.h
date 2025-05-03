//
//  simpleshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 21/03/2023.
//

#ifndef simpleshootingstyle_h
#define simpleshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const MELShootingStyleClass * _Nonnull SimpleShootingStyleGetClass(void);

void SimpleShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition);

#endif /* simpleshootingstyle_h */
