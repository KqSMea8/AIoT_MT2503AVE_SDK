/*
 * NeuroSky ECG extended algoirthm pipeline.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */


#include "nsk_defines.h"
#include "nsk_ecg_ext.h"
#include "nsk_ecg_core_state.h"
#include "nsk_ecg_core.h"
#include "hrv.h"
#include "mood.h"
#include "rsa.h"
#include "stress.h"
#include "ha.h"


#define NEL_RR_N (nsk_ecg_get_rri_count() > NEL_N_RR ? NEL_N_RR : nsk_ecg_get_rri_count())

extern int16_t g_nrr[NEL_N_RR];


int16_t nsk_ecg_compute_mood(void)
{
    nm_t m = {0};
    int16_t n = NEL_RR_N;
    if (n < 2) return -1;
    if (n < NEL_EXN) return -1;
    nel_mood_evaluate(&m, g_nrr+NEL_N_RR-n, n);
    return m.rm;
}


int16_t nsk_ecg_compute_hrv(void)
{
    int16_t a[NEL_N_RR] = {0};
    int16_t b[NEL_N_RR] = {0};
    int16_t n = NEL_RR_N;
    if (n < 2) return -1;
    if (n < NEL_EXN) return -1;
    return nel_hrv_kernel(g_nrr+NEL_N_RR-n, n, a, b);
}


int16_t nsk_ecg_compute_respiratory_rate(void)
{
    na_t ss;
    int16_t i;
    int16_t n = NEL_RR_N;
    if (n < 2) return -1;
    if (n < NEL_EXN) return -1;
    nel_rsa_init(&ss);
    for (i = 0; i < n; i++) {
        nel_rsa_add_sample(&ss, (int32_t)g_nrr[NEL_N_RR-n+i]*1000/NEL_FS);
    }
    return nel_rsa_compute(&ss);
}


int16_t nsk_ecg_compute_stress(
    const uint8_t feedback,
    const uint8_t gender,
    const uint8_t age,
    const uint16_t height,
    const uint16_t weight,
    int16_t* io_array)
{
    int16_t i;
    int16_t t[NEL_ST_RRI_LEN] = {0};
    if (NEL_RR_N < NEL_ST_RRI_LEN) return -1;
    for (i = 0; i < NEL_ST_RRI_LEN; i++) t[i] = 1000*(int32_t)g_nrr[NEL_N_RR-NEL_ST_RRI_LEN+i-1]/NEL_FS;
    return nel_stress(feedback, gender, age, height, weight, t, io_array);
}


int16_t nsk_ecg_compute_heart_age(const uint8_t age, int16_t* io_array)
{
    if (NEL_RR_N < NEL_EXN) return -1;
    return nel_heart_age(age, nsk_ecg_compute_hrv(), nsk_ecg_get_rri_count(), io_array);
}

int8_t nsk_ecg_get_heart_training_zone(
    const int8_t gender,   /* 0 for male, 1 for female */
    const int8_t age,      /* in years, 16-90 */
    const int16_t weight,  /* in kg, 1-30 */
    const int16_t hr       /* in bpm, 30-220 */
) {
    return nel_get_heart_training_zone( gender, age, weight, hr );
}

#undef NEL_RR_N

