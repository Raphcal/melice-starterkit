//
//  controller.c
//  Roll
//
//  Created by Raphaël Calabro on 23/03/2022.
//

#include "controller.h"

MELBoolean MELControllerEnabled = true;

static MELPoint toAxe(PDButtons buttons);

MELController MELControllerMake(void) {
    PDButtons current, pressed;
    playdate->system->getButtonState(&current, &pressed, NULL);

    MELController self = {
        .pressedA = pressed & kButtonA,
        .pressedB = pressed & kButtonB,
        .pressingA = current & kButtonA,
        .pressingB = current & kButtonB,
    };
    self.axe = toAxe(current);
    self.pressedAxe = toAxe(pressed);
    return self;
}

static MELPoint toAxe(PDButtons buttons) {
    return (MELPoint) {
        .x = (buttons & kButtonLeft)
            ? -1.0f
            : (buttons & kButtonRight)
                ? 1.0f : 0.0f,
        .y = (buttons & kButtonUp)
                ? -1.0f
                : (buttons & kButtonDown)
                    ? 1.0f : 0.0f,
    };
}

MELBoolean MELControllerIsIdle(const MELController self) {
    return !self.axe.x && !self.axe.y
        && !self.pressedA && !self.pressedB
        && !self.pressingA && !self.pressingB;
}
