/*
 * Heart beat R-peak detector.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file rpeak.h
 * @brief The R-peak detector.
 */

#ifndef RPEAK_H
#define RPEAK_H

#include "nsk_defines.h"

#define NEL_ND_N 8


/** The rpeak detector state stucture. */
typedef struct nd_s {
    int16_t buf[NEL_ND_N];
    uint8_t idx;
    uint32_t nw;
    uint32_t nx;
    uint32_t nd;
    int8_t iu;
    int8_t h1;
    int8_t h2;
    int8_t i1;
    int32_t dw;
    int32_t dr;
    int32_t ni;
    int32_t ai;
    int32_t mi;
} nd_t;


/**
 * Initalize the rpeak detector.
 */
void 
nel_rpeak_init(nd_t* ss);


/**
 * Add a new sample into rpeak detector.
 * @param[in, t] ss a pointer to rpeak detector state structure.
 * @param[in] x a sample of ECG.
 * @return the estimated R-R interval in number of samples, or @c -1 if no rpeak is detected.
 */
int16_t
nel_rpeak_add_sample(nd_t* ss, const int16_t x);


#endif

