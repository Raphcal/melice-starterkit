//
//  controller.h
//  Roll
//
//  Created by Raphaël Calabro on 23/03/2022.
//

#ifndef controller_h
#define controller_h

#include "melice.h"

typedef struct {
    MELPoint axe;
    MELPoint pressedAxe;
    MELBoolean pressedA;
    MELBoolean pressedB;
    MELBoolean pressingA;
    MELBoolean pressingB;
} MELController;

MELController MELControllerMake(void);

MELBoolean MELControllerIsIdle(const MELController self);

#endif /* controller_h */
