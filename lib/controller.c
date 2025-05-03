//
//  controller.c
//  Roll
//
//  Created by Raphaël Calabro on 23/03/2022.
//

#include "controller.h"

MELBoolean MELControllerEnabled = true;

MELController MELControllerMake(void) {
    PDButtons current, pressed;
    playdate->system->getButtonState(&current, &pressed, NULL);

    MELController self = {
        .pressedA = pressed & kButtonA,
        .pressedB = pressed & kButtonB,
        .pressingA = current & kButtonA,
        .pressingB = current & kButtonB,
    };
    if (current & kButtonLeft) {
        self.axe.x = -1.0f;
    } else if (current & kButtonRight) {
        self.axe.x = 1.0f;
    }
    if (current & kButtonUp) {
        self.axe.y = -1.0f;
    } else if (current & kButtonDown) {
        self.axe.y = 1.0f;
    }
    return self;
}
