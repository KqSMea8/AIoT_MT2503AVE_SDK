/*
 * Preprocessing band pass FIR filter, 8-35 Hz, 50 taps, integer coefficients.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file prep.h
 * @brief A 8-35Hz, 50 taps, integer c band-pass FIR filter for raw ECG.
 */

#ifndef PREP_H
#define PREP_H

#include "nsk_defines.h"

#define NEL_NP_N 50


/** The filter state structure. */
typedef struct np_s {
    int32_t il;
    int16_t buf[NEL_NP_N];
} np_t;


/** Initalize the filter state structure. */
void
nel_prep_init(np_t* ss);


/**
 * Filter a new sample.
 * @param[in, t] ss The filter state structure.
 * @param[in] x The sample to be filtered.
 * @return The filtered sample.
 */
int16_t
nel_prep_add_sample(np_t* ss, const int16_t x);


#endif

