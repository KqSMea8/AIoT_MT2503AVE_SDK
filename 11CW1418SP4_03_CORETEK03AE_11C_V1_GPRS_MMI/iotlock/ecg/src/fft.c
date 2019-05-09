/*
 * Fixed point fft, radix 2, max length 256.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-25-2015;
 */

#include <string.h>
#include "nsk_defines.h"
#include "fft.h"

#define NEL_FT_N 256
#define NEL_FT_D 8
#define NEL_FT_M(a, b) (((int32_t)(a)*(int32_t)(b))>>15)


#if (defined NEL_USE_DIVISION_FOR_RSHIFT && NEL_USE_DIVISION_FOR_RSHIFT == 1)
    #define NEL_FT_S(a) ((a)/2)
#else
    #define NEL_FT_S(a) ((a)>>1)
#endif


static const int16_t NEL_FF_SIN[NEL_FT_N*3/4] = {
    0, 804, 1608, 2410, 3212, 4011, 4808, 5602, 6393, 7179, 7962, 8739, 9512, 10278, 11039, 11793, 12539, 13279,
    14010, 14732, 15446, 16151, 16846, 17530, 18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594, 23170, 23731,
    24279, 24811, 25329, 25832, 26319, 26790, 27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956, 30273, 30571,
    30852, 31113, 31356, 31580, 31785, 31971, 32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757, 32767, 32757,
    32728, 32678, 32609, 32521, 32412, 32285, 32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571, 30273, 29956,
    29621, 29268, 28898, 28510, 28105, 27683, 27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731, 23170, 22594,
    22005, 21403, 20787, 20159, 19519, 18868, 18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279, 12539, 11793,
    11039, 10278, 9512, 8739, 7962, 7179, 6393, 5602, 4808, 4011, 3212, 2410, 1608, 804, 0, -804, -1608, -2410,
    -3212, -4011, -4808, -5602, -6393, -7179, -7962, -8739, -9512, -10278, -11039, -11793, -12539, -13279, -14010,
    -14732, -15446, -16151, -16846, -17530, -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594, -23170,
    -23731, -24279, -24811, -25329, -25832, -26319, -26790, -27245, -27683, -28105, -28510, -28898, -29268, -29621,
    -29956, -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971, -32137, -32285, -32412, -32521, -32609,
    -32678, -32728, -32757
};

/* in-place fft; fr real part, fi imag part; n is length of input, must be power of 2 and less than NEL_FT_N */
void nel_fix_fft(int16_t* fr, int16_t* fi, int16_t n)
{
    int32_t mr = 0;
    int32_t nn = 0;
    int32_t i = 0;
    int32_t j = 0;
    int32_t v = 0;
    int32_t k = 0;
    int32_t iz = 0;
    int32_t m = 0;
    int16_t qr = 0;
    int16_t qi = 0;
    int16_t tr = 0;
    int16_t ti = 0;
    int16_t wr = 0;
    int16_t wi = 0;

    if (n > NEL_FT_N) return;

    mr = 0;
    nn = n-1;

    for (m = 1; m <= nn; ++m) {
        v = n;

        do v = NEL_FT_S(v); while (mr+v > nn);

        mr = (mr&(v-1))+v;
        if (mr <= m) continue;

        tr = fr[m];
        fr[m] = fr[mr];
        fr[mr] = tr;
        ti = fi[m];
        fi[m] = fi[mr];
        fi[mr] = ti;
    }

    v = 1;
    k = NEL_FT_D-1;
    while (v < n) {
        iz = v<<1;

        for (m = 0; m < v; ++m) {
            j = m<<k;            
            wr = NEL_FT_S(NEL_FF_SIN[j+64]);
            wi = NEL_FT_S(-NEL_FF_SIN[j]);

            for (i = m; i < n; i += iz) {
                j = i+v;
                tr = NEL_FT_M(wr, fr[j])-NEL_FT_M(wi, fi[j]);
                ti = NEL_FT_M(wr, fi[j])+NEL_FT_M(wi, fr[j]);
                
                qr = NEL_FT_S(fr[i]);
                qi = NEL_FT_S(fi[i]);

                fr[j] = qr-tr;
                fi[j] = qi-ti;
                fr[i] = qr+tr;
                fi[i] = qi+ti;
            }
        }
        --k;
        v = iz;
    }
}

/* x is all real int16 input, m is length of x. After fft, real part is in index 0..m/2-1 and imag part m/2..m-1 */
void nel_fix_fft_real(int16_t* x, int16_t m)
{
    int16_t s = 0;
    int16_t c = 0;
    int16_t i = 0;
    int16_t n = m/2;
    int32_t t1r = 0;
    int32_t t1i = 0;
    int32_t t2r = 0;
    int32_t t2i = 0;
    int16_t b[NEL_FT_N] = { 0 };
    int16_t* fr = x;
    int16_t* fi = &x[n];

    for (i = 0; i < n; i++) {
        b[i] = x[i+i];
        b[n+i] = x[i+i+1];
    }

    memcpy(x, b, sizeof(int16_t)*m);

    nel_fix_fft(fr, fi, n);

    for (i = 1; i <= n/2; i++) {
        s = NEL_FF_SIN[i];
        c = NEL_FF_SIN[i+64];

        t1r = fr[i];
        t1i = fi[i];
        t2r = fr[n-i];
        t2i = fi[n-i];

        fr[i] = NEL_FT_S((t1r+t2r) + NEL_FT_M(c, (t1i+t2i)) - NEL_FT_M(s, (t1r-t2r)));
        fi[i] = NEL_FT_S((t1i-t2i) - NEL_FT_M(c, (t1r-t2r)) - NEL_FT_M(s, (t1i+t2i)));
        fr[n-i] = NEL_FT_S((t1r+t2r) - NEL_FT_M(c, (t1i+t2i)) + NEL_FT_M(s, (t1r-t2r)));
        fi[n-i] = NEL_FT_S((-t1i+t2i) - NEL_FT_M(c, (t1r-t2r)) - NEL_FT_M(s, (t1i+t2i)));
    }

    t1r = fr[0];
    t1i = fi[0];
    fr[0] = t1r+t1i;
    fi[0] = t1r-t1i;
}

