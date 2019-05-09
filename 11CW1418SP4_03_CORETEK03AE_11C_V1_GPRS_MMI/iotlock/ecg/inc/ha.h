/*
 * Heart Age;
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 07-13-2015;
 */

/**
 * @file ha.h
 * @brief ECG Heart Age.
 */


#ifndef NEL_HA_H
#define NEL_HA_H

#include "nsk_defines.h"

/**
 * Heart Age.
 * @param[in] age, the users true age, 16-90 years.
 * @param[in] hrv, the current HRV value in ms.
 * @param[in] rrc, the RR interval count at which the HRV has been calculated.
 * @param[in, t] io_array, a 16 bit integer array of length 128. This is the hisotrical data. The user is responsible
 * for maintaining this array (save, load ... etc) for future use.
 * @return the heart age, 16-105 years.
 */
uint8_t
nel_heart_age(const uint8_t age, const int16_t hrv, const int16_t rrc, int16_t* io_array);


#endif

