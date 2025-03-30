//
//  random.h
//  shmup
//
//  Created by Raphaël Calabro on 15/03/2019.
//  Copyright © 2019 Raphaël Calabro. All rights reserved.
//

#ifndef random_h
#define random_h

#include "melstd.h"

/**
 * Initialize the random value generator.
 */
void MELRandomInit(void);

/**
 * Returns an integer located between 0 and range - 1
 *
 * @param range maximum value (excluded).
 * @return A pseudo-random decimal number.
 */
int MELRandomInt(int range);

/**
 * Returns a decimal number located between 0.0 and range - 0.0...1
 *
 * @param range maximum value (excluded).
 * @return A pseudo-random decimal number.
 */
float MELRandomFloat(float range);

/**
 * Returns a decimal number located between 0.0 and range - 0.0...1
 *
 * @param range maximum value (excluded).
 * @return A pseudo-random decimal number.
 */
MELTimeInterval MELRandomTimeInterval(MELTimeInterval range);

/**
 * Returns a pseudo-random integer located between lower and upper (included).
 */
int MELRandomIntWithRange(int lower, int upper);

/**
 * Returns a pseudo-random decimal number located between lower and upper - 0.000...1
 */
float MELRandomFloatWithRange(float lower, float upper);

/**
 * Returns a pseudo-random decimal number located between lower and upper - 0.000...1
 */
MELTimeInterval MELRandomTimeIntervalWithRange(MELTimeInterval lower, MELTimeInterval upper);

#endif /* random_h */
