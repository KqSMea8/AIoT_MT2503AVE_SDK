/*
 * ECG Spectral Engergy Measures;
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-25-2015;
 */

/**
 * @file spectr.h
 * @brief ECG Spectral Energy.
 */

#ifndef NEL_SPECTR_H
#define NEL_SPECTR_H

#include "nsk_defines.h"

/**
 * Compute High and Low freq power from an array of RR interval values (in sample unit).
 * @param[in] br the RRI array in sample unit.
 * @param[in] lr the length of RRI array.
 * @param[t] pl the low frequency power.
 * @param[t] ph the high frequency power.
 */
void
nel_sem_compute_hflf(
    const int16_t* br,
    const int32_t lr,
    uint32_t* pl,
    uint32_t* ph);


/** Compute RX value from low and high frequency power. */
int16_t
nel_sem_rx(uint32_t pl, uint32_t ph);


#endif

