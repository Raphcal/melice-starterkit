//
//  random.c
//  shmup
//
//  Created by Raphaël Calabro on 15/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#include "random.h"

#define RANDOM_MAXIMUM RAND_MAX
#define RANDOM_FUNCTION rand

void MELRandomInit(void) {
    srand(playdate->system->getCurrentTimeMilliseconds());
}

int MELRandomInt(int range) {
    return RANDOM_FUNCTION() % range;
}

float MELRandomFloat(float range) {
    return (float)((double)RANDOM_FUNCTION() / (double)RANDOM_MAXIMUM) * range;
}

MELTimeInterval MELRandomTimeInterval(MELTimeInterval range) {
    return (float)((double)RANDOM_FUNCTION() / (double)RANDOM_MAXIMUM) * range;
}

int MELRandomIntWithRange(int lower, int upper) {
    return MELRandomInt(upper - lower + 1) + lower;
}

float MELRandomFloatWithRange(float lower, float upper) {
    return MELRandomFloat(upper - lower) + lower;
}

MELTimeInterval MELRandomTimeIntervalWithRange(MELTimeInterval lower, MELTimeInterval upper) {
    return MELRandomTimeInterval(upper - lower) + lower;
}
