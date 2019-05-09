/*
 * Noise checker.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file noise.h
 * @brief Mechanical noise checker. The baseline must be adjusted before usage.
 */

#ifndef NEL_NOISE_H
#define NEL_NOISE_H

#include "nsk_defines.h"

#define NEL_NN_LX 64
#define NEL_NN_LW 8

/** The noise checker state structure. */
typedef struct nn_s {
    uint8_t ix;
    uint8_t ir;
    uint8_t ip3;
    uint8_t io2;
    uint8_t ip2;
    int16_t bx[NEL_NN_LX];
    int16_t br[NEL_NN_LX];
    int16_t bp2[NEL_NN_LW];
    int16_t bp3[NEL_NN_LW];
    int16_t bo2[NEL_NN_LW];
    int16_t xd;
    int32_t t1;
    int32_t t2;
    int32_t t3;
    int32_t ns;
    int32_t nz;
    int32_t np2;
    int32_t np3;
    int32_t no2;
    int32_t nx;
} nn_t;


/** Initialize the noise checker */
void
nel_noise_init(nn_t* ss);


/**
 * Add a new sample to the noise checker.
 * @param[in, t] ss the noise checker state data structure.
 * @param[in] x a new data sample.
 * @return the estimated noise level for @a ss->xd. Any value greater than @c 0 indicates presence of noise.
 */
int16_t
nel_noise_add_sample(nn_t* ss, const int16_t x);

#endif

