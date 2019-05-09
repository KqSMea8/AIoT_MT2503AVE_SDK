/*
 * Respiratory rate.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 04-15-2015;
 */

/**
 * @file rsa.h
 * @brief Respiratory rate, uses time domain method to compute respiratory rate from a serie of RR intervals.
 */

#ifndef NEL_RSA_H
#define NEL_RSA_H

#include "nsk_defines.h"

#define NEL_RS_L 8
#define NEL_RS_U 30


/** The RSA state structure. */
typedef struct na_s {
    int32_t xc;
    int32_t x0;
    int32_t np;
    int32_t nv;
    int32_t nt;
    int32_t cf;
    int32_t lk;
    int8_t pv;
} na_t;


/**
 * Initialize the state structure.
 * @param[in, t] ss the state structure.
 */
void
nel_rsa_init(na_t* ss);


/**
 * Add a new RRI sample to update the RSA state structure.
 * @param[in, t] ss the state structure.
 * @param[in] x the input RRI sample, in unit of milliseconds.
 * @return the sample index of last detected extrema, or 0 if no extrema is detected.
 */
int32_t
nel_rsa_add_sample(na_t* ss, const int32_t x);


/**
 * Compute the Respiratory rate from the current state structure.
 * @param[in] ss the state structure.
 * @return the respiratory rate estimation, in unit of breath per minute.
 */
int32_t
nel_rsa_compute(na_t* ss);


#endif

