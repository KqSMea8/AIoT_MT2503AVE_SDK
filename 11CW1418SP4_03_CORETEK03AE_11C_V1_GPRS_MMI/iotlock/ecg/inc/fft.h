/*
 * Fixed point fft, radix 2, max length 256.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-27-2015;
 */

/**
 * @file fft.h
 * @brief An integer FFT routine up to length of 256. 
 */
 
#ifndef NEL_FFT_H
#define NEL_FFT_H

#include "nsk_defines.h"

/**
 * In-place complex-to-complex FFT.
 * @param[in, t] fr the real part of data, time domain before FFT and frequency domain after. 
 * @param[in, t] fi the imaginary part, time domain before FFT and frequency domain after. 
 * @param[in] n the length of data array, for both @a fr and @a fi .
 */
void 
nel_fix_fft(
    int16_t* fr, 
    int16_t* fi, 
    int16_t n
);

/**
 * In-place real-to-complex FFT.
 * @param[in, t] x the real valued data in time domain before FFT.
 *     After FFT, the DC is the index 0 element, and the real-part half-spectrum is elements index 1 to m/2-1.
 *     The imaginary part half-spectrum is elements index m/2 to m-1.
 * @param[in] m the length of data array @a x.
 */
void 
nel_fix_fft_real(
    int16_t* x, 
    int16_t m
);

#endif

