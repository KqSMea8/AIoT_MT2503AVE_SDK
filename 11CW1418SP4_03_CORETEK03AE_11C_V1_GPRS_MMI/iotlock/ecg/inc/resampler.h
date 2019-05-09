/*
 * Raw ECG resampling.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */

/**
 * @file resampler.h
 * @brief The ECG resampling using linear interpolation.
 */

#ifndef RESAMPLER_H
#define RESAMPLER_H

#include "nsk_defines.h"

/** The resampler state structure. */
typedef struct nr_s {
    int16_t y[2];
    int16_t x;
    uint16_t fs;
    uint16_t ofs;
    uint32_t a1;
    uint32_t a2;
} nr_t;


/**
 * Initialize the resampler.
 * @param[in, t] s the state structure.
 * @param[in] fs the sampling rate of input data.
 * @param[in] ofs the sampling rate of output data.
 */
void
nel_resampler_init(nr_t* s, const uint16_t fs, const uint16_t ofs);


/**
 * Add a raw sample to the resampler and perform resampling.
 * @param[in, t] s the state structure.
 * @param[in] x raw ECG sample.
 * @return number of output, the resampled output is in @c s.y.
 */
int8_t
nel_resampler_add_sample(nr_t* s, const int32_t x);

#endif
