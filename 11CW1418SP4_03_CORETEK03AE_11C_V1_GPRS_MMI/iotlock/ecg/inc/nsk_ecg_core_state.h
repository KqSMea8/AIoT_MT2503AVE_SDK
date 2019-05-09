/*
 * Core ECG algorithm pipeline state structure.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */

/**
 * @file nsk_ecg_core_state.h
 * @brief The Core ECG algorithm pipeline state structure.
 */


#ifndef NSK_ECG_CORE_STATE_H
#define NSK_ECG_CORE_STATE_H

#include "nsk_defines.h"
#include "param.h"
#include "median.h"
#include "resampler.h"
#include "prep.h"
#include "rpeak.h"
#include "noise.h"
#include "sq.h"
#include "rhr.h"


#define NEL_N_RR 256

#define NEL_TSQ 3
#define NEL_NW 9
#define NEL_AN 11
#define NEL_AF 11


/** The pipeline state structure. */
typedef struct nc_s {
    uint16_t fs;
    uint8_t nq;
    uint8_t nl;
    int16_t rl;
    int16_t nr;
    int32_t ne;
    uint8_t hu;

    nr_t ssr;
    np_t ssp;
    nn_t ssn;
    nd_t ssd;
    nq_t ssq;
    nh_t ssh;
} nc_t;

/* Functions below are for internal use only, undocumentted. */
void nel_init(nc_t* ss, int16_t* rri);
int16_t nel_compute_hr(nc_t* ss, int16_t* rri);
void nel_update(nc_t* ss, int16_t* rri, int16_t x);
void nel_smooth_rri(nc_t* ss, int16_t* rri);

#endif

