/*
 * Integer maths.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file imath.h
 * @brief Contains the integer version of some common maths and statistical functions.
 */

#ifndef IMATH_H
#define IMATH_H

#include "nsk_defines.h"

/**
 * Integer square root.
 *     Computes the approximate square root of interger input @a x.
 */
uint32_t
nel_isqrt(const uint32_t x);

/**
 * Integer mean.
 *     Computes the approximate mean of input integer array @a x with array length specified by @a n.
 */
int32_t
nel_imean(
    const int32_t* x, 
    const int32_t n
);

/**
 * Integer variance.
 *     Computes the approximate variance of input integer array @a x with array length specified by @a n.
 */
uint32_t
nel_ivariance(
    const int32_t* x, 
    const int32_t n
);

#endif
