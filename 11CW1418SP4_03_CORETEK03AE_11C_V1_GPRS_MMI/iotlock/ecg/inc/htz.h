/*
 * ECG Heart Training Zone
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Kelvin Soo, Aware Shi; 2017-07-08;
 */

/**
 * @file htz.h
 * @brief ECG Heart Training Zone
 */

#ifndef NEL_HTZ_H
#define NEL_HTZ_H

#include "nsk_defines.h"

/**
 * Excercise is weight-efficient when it is performed within
 * a certain range of heart rates during exercise.
 * For achieving weight loss, exercise should be in the 2~3 
 * zones, and last more than 45 minutes of time (it takes
 * about 40 minutes to achieve glucose depletion and start
 * burning fat for energy), but preferably not more than an
 * hour.
 *
 * @return The current Heart Training Zone:
 *             -1 - invalid argument(s)
 *              0 - no training
 *              1 - entering minimal training zone; serves as warmup
 *                  before exercise or keeping in shape
 *              2 - fat burning zone (aerobic exercise); achieving
 *                  general fitness and fat burn
 *              3 - glycogen consumption zone; training lung function
 *                  and endurance
 *              4 - anaerobic training zone; for boosting fitness,
 *                  speed, and power
 *              5 - maximum zone; at the typical limits of human
 *                  high-intensity exercise
 */
int8_t 
nel_get_heart_training_zone( 
    const int8_t gender,   /* 0 for male, 1 for female */
    const int8_t age,      /* in years, 16-90 */
    const int16_t weight,  /* in kg, 1-300 */
    const int16_t hr       /* in bpm, 30-220 */
);

#endif