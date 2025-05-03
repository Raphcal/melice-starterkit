//
//  particuleshootingstyle.h
//  Kuroobi
//
//  Created by Raphaël Calabro on 22/04/2023.
//

#ifndef particuleshootingstyle_h
#define particuleshootingstyle_h

#include "shootingstyle.h"
#include "shootingstyledefinition.h"

const MELShootingStyleClass * _Nonnull ParticuleShootingStyleGetClass(void);

void ParticuleShootingStyleInit(MELShootingStyle * _Nonnull self, const MELShootingStyleDefinition * _Nonnull definition);

#endif /* particuleshootingstyle_h */
