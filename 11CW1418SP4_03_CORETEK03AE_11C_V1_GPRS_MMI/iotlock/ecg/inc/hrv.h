/*
 * Robust HRV estimator.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-19-2015;
 */

/**
 * @file hrv.h
 * @brief A robust HRV estimator that is less sensitive to abrupt outliers in the input array.
 */

#ifndef NEL_HRV_H
#define NEL_HRV_H

#include "nsk_defines.h"

#define NEL_HRV_L 1
#define NEL_HRV_U 300

/**
 * Compute NeuroSky HRV from an array of RR Interval values (in sample unit).
 * @param[in] x, the array of RRI in sample unit.
 * @param[in] n, the length of the above array.
 * @param[in, t] z an augmentary array of length n, for intermediate calculation.
 * @param[in, t] y an augmentary array of length n, for intermediate calculation.
 * @return the computed HRV value in unit of milliseconds.
 */
int16_t
nel_hrv_kernel(const int16_t* x, const int32_t n, int16_t* z, int16_t* y);

#endif

