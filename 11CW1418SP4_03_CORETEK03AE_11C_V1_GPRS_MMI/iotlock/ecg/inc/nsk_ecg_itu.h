/*
 * NeuroSky ECG library internal testing utilities.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 04-13-2015;
 */

/**
 * @file nsk_ecg_itu.h
 * @brief NeuroSky ECG library internal testing utilities.
 */

#ifndef _WIN32
#pragma GCC visibility push(default)
#endif

#ifndef NSK_ECG_ITU_H
#define NSK_ECG_ITU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nsk_defines.h"

char* nsk_ecg_get_lib_signature(void);

void* nsk_ecg_get_state(void);

int16_t nsk_ecg_get_ecg_delayed(void);

#ifdef __cplusplus
}
#endif

#endif

#ifndef _WIN32
#pragma GCC visibility pop
#endif

