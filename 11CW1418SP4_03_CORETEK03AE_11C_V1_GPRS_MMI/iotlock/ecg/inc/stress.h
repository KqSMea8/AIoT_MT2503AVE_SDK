/*
 * ECG Stress;
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 07-07-2015;
 */

/**
 * @file stress.h
 * @brief ECG Stress.
 */

#ifndef NEL_STRESS_H
#define NEL_STRESS_H

#include "nsk_defines.h"

#define NEL_ST_RRI_LEN 30

/**
 * Compute stress.
 * @return the stress value, from 1-100, the larger the more stressful.
 */
int32_t
nel_stress(
    const uint8_t feedback, /**< user feedback for previous session, 1-100. Set to 0 if no feedback. */
    const uint8_t gender, /**< 0 for male, 1 for female. */
    const uint8_t age, /**< in years, 16-90. */
    const uint16_t height, /**< in cm, 1-300. */
    const uint16_t weight, /**< in kg, 1-300. */
    const int16_t* rri, /** An short integer array of 30 RRIs in ms. */
    int16_t* io_array /**< An short integer array of length @c NEL_IOG. */
);

#endif
