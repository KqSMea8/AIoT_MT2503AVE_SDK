/*
 * Signal Quality.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file sq.h
 * @brief Signal quality measure. Range from 1-5, with 5 being the best quality, 1 the worst;
 * The algorithm checks four different aspect of the signal for the quality measure: the mechanical noise, 
 * The RRI consistency, the beat signal consistency, and the R-peak height relative to other part of a beat.
 * The signal quality measure is determined by the lowest of all these four aspects.
 * When the algorithm is first initialized and not yet settled, sq of -1 will be returned.
 * Delay against input raw is 48 samples.
 */

#ifndef NEL_SQ_H
#define NEL_SQ_H

#include "nsk_defines.h"

#define NEL_NQ_N 64
#define NEL_NQ_M 2
#define NEL_NQ_D 48

/** The Signal quality (SQ) algorithm state structure. */
typedef struct nq_s {
    uint8_t hu;
    int16_t ny;
    int16_t s0[NEL_NQ_N];
    int16_t s1[NEL_NQ_N];
    int16_t r0;
    int16_t r1;
    int16_t ql;
    int32_t nb;
    int32_t nn;
    int32_t c1;
    int32_t c2;
    int32_t c3;
    int32_t lsq;
    uint32_t a0;
    uint32_t a1;
} nq_t;


/**
 * Initialize the SQ algorithm.
 * @param[in, t] ss the SQ state structure.
 */
void
nel_sq_init(nq_t* ss);


/**
 * Add a data sample (presumably preprocessed), and estimate the signal quality of the moment.
 * the output is within range 1-5; it is delayed against the input data by 48 samples.
 * When algorithm is not yet settled, the returned value will be 0.
 * @param[in, t] ss the SQ state structure.
 * @param[in] x the preprocessed input data sample.
 * @param[in] rri rri result of the beat detector for the same input x.
 * @param[in] iz noise check result for the same input x.
 * @return signal qualtiy estimate of the current moment.
 */
int16_t
nel_sq_add_sample(nq_t* ss, const int16_t x, const int16_t rri, const uint8_t iz);


#endif

