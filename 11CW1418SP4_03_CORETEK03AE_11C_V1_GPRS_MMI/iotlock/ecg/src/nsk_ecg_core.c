/*
 * NeuroSky ECG core algoirthm pipeline.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */


#include "nsk_defines.h"
#include "nsk_ecg_core.h"
#include "nsk_ecg_core_state.h"
#include "version.h"
#include "sig.h"


static nc_t s_ncs = {NEL_FS*2, 0};

int16_t g_nrr[NEL_N_RR] = {0};

char* nsk_ecg_get_lib_version_string(void)
{
    return NSK_ECG_LIB_VERSION;
}


char* nsk_ecg_get_lib_signature(void)
{
    return NSK_ECG_LIB_SIGNATURE;
}


void* nsk_ecg_get_state(void)
{
    return (void*)(&s_ncs);
}


void nsk_ecg_set_raw_sample_rate(const uint16_t fs)
{
    if (fs >= 128 && fs <= NEL_FS*20) s_ncs.fs = fs;
    else s_ncs.fs = NEL_FS*2;
}


void nsk_ecg_init(void)
{
    nel_init(&s_ncs, g_nrr);
}


uint8_t nsk_ecg_is_new_beat(void)
{
    return s_ncs.hu;
}


int16_t nsk_ecg_get_ecg_delayed(void)
{
    return s_ncs.ssq.s0[17];
}


int16_t nsk_ecg_get_signal_quality(void)
{
    return s_ncs.ssq.ql;
}


int16_t nsk_ecg_get_overall_signal_quality(void)
{
    return s_ncs.ssq.lsq;
}


int16_t nsk_ecg_get_rri(void)
{
    return 1000*(int32_t)g_nrr[NEL_N_RR-1]/NEL_FS;
}


int32_t nsk_ecg_get_rri_count(void)
{
    return s_ncs.nr;
}


int32_t nsk_ecg_get_sample_count(void)
{
    return s_ncs.ne;
}


int16_t nsk_ecg_get_hr_robust(void)
{
    return s_ncs.ssh.r1;
}


int16_t nsk_ecg_rescale_adc(const int16_t x, const int32_t adc)
{
    if (adc == NEL_ADC) return x;
    return (adc > 0 ? ((int32_t)x)*NEL_ADC/adc : 0);
}


int16_t nsk_ecg_compute_hr(void)
{
    return nel_compute_hr(&s_ncs, g_nrr);
}


void nsk_ecg_update(const int16_t x)
{
    nel_update(&s_ncs, g_nrr, x);
}



