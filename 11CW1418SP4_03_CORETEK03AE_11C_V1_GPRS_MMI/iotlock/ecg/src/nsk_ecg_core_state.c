/*
 * NeuroSky ECG core algoirthm pipeline state functions.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-17-2015; 04-21-2015;
 */

#include <string.h>
#include "nsk_defines.h"
#include "nsk_ecg_core_state.h"


void nel_init(nc_t* ss, int16_t* rri)
{
    ss->nq = 0;
    ss->nl = 0;
    ss->rl = 0;
    ss->nr = 0;
    ss->ne = 0;
    ss->hu = 0;

    memset(rri, 0, sizeof(int16_t)*NEL_N_RR);

    nel_resampler_init(&(ss->ssr), ss->fs, NEL_FS);
    nel_prep_init(&(ss->ssp));
    nel_noise_init(&(ss->ssn));
    nel_rpeak_init(&(ss->ssd));
    nel_rhr_init(&(ss->ssh), NEL_TSQ);
    nel_sq_init(&(ss->ssq));
}


int16_t nel_compute_hr(nc_t* ss, int16_t* rri)
{
    int32_t n = ss->nr > NEL_N_RR ? NEL_N_RR : ss->nr;
    int16_t w[NEL_NW] = {0};
    if (n < 1) return -1;
    if (n > NEL_NW) {
        memcpy(w, rri+NEL_N_RR-NEL_NW, sizeof(int16_t)*NEL_NW);
        n = NEL_NW;
    }
    else {
        memcpy(w, rri+NEL_N_RR-n, sizeof(int16_t)*n);
    }
    n = nel_quick_median(w, n);
    n = n < 1 ? 1 : n;
    return 60*NEL_FS/(int32_t)n;
}


void nel_smooth_rri(nc_t* ss, int16_t* rri)
{
    int16_t mri = nel_compute_hr(ss, rri);
    int16_t thr;
    int16_t i;
    if (mri == 0) return;
    mri = 60*NEL_FS/(int32_t)mri;
    thr = mri*3/10;
    if (ss->nr == NEL_AN+1) {
        for (i = NEL_N_RR-NEL_AN-1; i < NEL_N_RR; i++) {
            if (rri[i] > (mri+thr) || rri[i] < (mri-thr)) rri[i] = mri-(thr>>3)+((ss->ne+i)%(thr>>2));
        }
    }
    else {
        i = NEL_N_RR-1;
        if (rri[i] > (mri+thr) || rri[i] < (mri-thr)) rri[i] = mri-(thr>>3)+((ss->ne+i)%(thr>>2));
    }
}


void nel_update(nc_t* ss, int16_t* rri, int16_t x)
{
    int16_t g1;
    int16_t g2 = 0;
    int16_t g3 = -1;
    int8_t i, n;

    ss->hu = 0;
    ss->ne++;

    n = nel_resampler_add_sample(&(ss->ssr), x);
    if (n == 0) return;

    for (i = 0; i < n; i++) {
        g1 = nel_prep_add_sample(&(ss->ssp), ss->ssr.y[i]);
        g2 = nel_noise_add_sample(&(ss->ssn), g1);
        g3 = -1;

        if (g2 == 0) {
            g3 = nel_rpeak_add_sample(&(ss->ssd), ss->ssn.xd);
        }

        nel_sq_add_sample(&(ss->ssq), ss->ssn.xd, g3, g2 > 0);

        if (g3 > 0) {
            ss->nq = NEL_NQ_D-1;
            ss->rl = g3;
        }

        if (ss->nq > 0) {
            ss->nq--;
            if ((ss->nq == 0) &&
                    !(ss->nl > 0 && g2 == 0 && ss->ssq.ql <= NEL_TSQ)) {
                memmove(rri, rri+1, (NEL_N_RR-1)*sizeof(int16_t));
                rri[NEL_N_RR-1] = ss->rl;
                ss->nr++;
                ss->hu = 1;
                if (ss->nr > NEL_AN && (ss->nr%NEL_AF) == 0) {
                    nel_smooth_rri(ss, rri);
                }
            }
        }
    }

    if (ss->hu) {
        nel_rhr_update(&(ss->ssh),
                ss->ssq.ql, nel_compute_hr(ss, rri), ss->ne);
    }

    ss->nl = g2 > 0;
}

