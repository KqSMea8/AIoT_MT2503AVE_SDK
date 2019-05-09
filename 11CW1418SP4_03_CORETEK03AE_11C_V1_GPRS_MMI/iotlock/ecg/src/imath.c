/* 
 * Integer maths. 
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

#include "nsk_defines.h"

#define ITER1(N) \
    test = root+(1<<(N)); \
    if (n >= test<<(N)) { \
        n -= test<<(N); \
        root |= 2<<(N); \
    }

/* Square root. */
uint32_t nel_isqrt(const uint32_t x) 
{
    uint32_t root = 0;
    uint32_t test;
    uint32_t n = x;
    ITER1(15);    ITER1(14);    ITER1(13);    ITER1(12);
    ITER1(11);    ITER1(10);    ITER1(9);     ITER1(8);
    ITER1(7);     ITER1(6);     ITER1(5);     ITER1(4);
    ITER1(3);     ITER1(2);     ITER1(1);     ITER1(0);
    return root>>1;
}

#undef ITER1

/* Mean. */
int32_t nel_imean(const int32_t* x, const int32_t n) 
{
    int32_t i;
    int32_t mean = 0;

    if (n < 1) return 0;

    for (i = 0; i < n; i++) {
        mean += x[i];
    }

    return mean /= n;
}

/* Variance. */
uint32_t nel_ivariance(const int32_t* x, const int32_t n)
{
    int32_t i;
    int32_t s;
    int32_t mean;
    uint32_t var = 0;

    if (n < 2) return 0;

    mean = nel_imean(x, n);
    for (i = 0; i < n; i++) {
        s = x[i]-mean;
        var += s*s;
    }

    return var /= n-1;
}

