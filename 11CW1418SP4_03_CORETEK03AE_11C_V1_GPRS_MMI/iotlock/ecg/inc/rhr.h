/*
 * Heart Rate.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-26-2015;
 */

/**
 * @file rhr.h
 * @brief Robust Heart Rate, which utilize the SQ to weight the HR to be more robust in noisy condition.
 */


#ifndef NEL_RHR_H
#define NEL_RHR_H

#include "nsk_defines.h"


/** The RHR state structure. */
typedef struct nh_s {
    int32_t r0;
    int16_t r1;
    uint8_t th;
} nh_t;


/**
 * Initialize the state structure.
 * @param[in, t] ss the state structure.
 * @param[in] th the threshold to judge Signal Quality. An integer value from 1-5;
 */
void
nel_rhr_init(nh_t* ss, uint8_t th);


/**
 * Update the robust Heart Rate calculation.
 * @param[in, t] ss the state structure.
 * @param[in] sq the current signal quality, returned from SQ algorithm.
 * @param[in] hr the current heart rate.
 * @param[in] c the current ecg sample count.
 */
void
nel_rhr_update(nh_t* ss, int16_t sq, int16_t hr, int32_t c);

#endif


