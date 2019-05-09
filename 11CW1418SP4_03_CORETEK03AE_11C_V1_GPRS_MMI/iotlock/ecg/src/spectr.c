/*
 * ECG Spectral Engergy;
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-25-2015;
 */

#include <stdlib.h>
#include <string.h>
#include "nsk_defines.h"
#include "fft.h"
#include "spectr.h"

#define NEL_SE_NS 33
#define NEL_SE_NX 33
#define NEL_SE_NF 256
#define NEL_SE_NT 64
#define NEL_SE_S1 1
#define NEL_SE_S2 8
#define NEL_SE_X1 1
#define NEL_SE_X2 100
#define NEL_SE_TS 3
#define NEL_SE_B1 880
#define NEL_SE_B2 1320
#define NEL_SE_B3 1760
#define NEL_SE_B4 2640
#define NEL_SE_B5 3
#define NEL_SE_B6 10
#define NEL_SE_B7 27

#if (defined NEL_USE_DIVISION_FOR_RSHIFT && NEL_USE_DIVISION_FOR_RSHIFT == 1)
    #define NEL_SE_SC(a) ((a)/4)
#else
    #define NEL_SE_SC(a) ((a)>>2)
#endif

static const uint16_t NEL_SE_LRX[NEL_SE_NX] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 15, 17, 18, 20, 22, 24, 27,
    29, 33, 36, 41, 46, 52, 61, 72, 87, 109, 146, 220, 441
};


void nel_sem_compute_hflf(
    const int16_t* br,
    const int32_t lr,
    uint32_t* pl,
    uint32_t* ph)
{
    uint32_t i;
    uint32_t k;
    uint32_t ft;
    uint32_t gt;
    int16_t f[NEL_SE_NF] = {0};
    uint32_t trr;
    uint32_t tit;
    int32_t of;
    int32_t rit;

    *pl = 0;
    *ph = 0;

    f[0] = br[0];
    trr = br[0];
    tit = br[0];
    k = 1;

    for (i = 1; i < (uint32_t)lr; i++) {
        trr += br[i];

        while (k*NEL_SE_NT <= trr) {
            of = trr-k*NEL_SE_NT;

            if (of == 0) {
                rit = br[i];
            }
            else {
                rit = (br[i]-of)*(br[i]-br[i-1])
                        /(br[i] == 0 ? 1 : br[i])+br[i-1];
            }

            f[k] = rit;
            tit += rit;
            k++;
            if (k > NEL_SE_NF-1) goto label_interp_done;
        }
    }

label_interp_done:
    k--;
    if (k < 1) k = 1;
    tit /= k;

    for (i = 0; i < k; i++) {
        f[i] = (f[i]-tit)<<6;
    }

    nel_fix_fft_real(f, NEL_SE_NF);

    for (i = NEL_SE_B5; i < NEL_SE_NS; ++i) {
        ft = NEL_SE_SC((uint32_t)abs(f[i]));
        gt = NEL_SE_SC((uint32_t)abs(f[i+NEL_SE_NF/2]));
        ft = NEL_SE_SC(ft*ft+gt*gt);

        if (i < NEL_SE_B6) {
            *pl += ft;
        }
        else if (i < NEL_SE_B7) {
            *ph += ft;
        }
    }

    *ph *= NEL_SE_S1;
    *pl *= NEL_SE_S2;
}


int16_t nel_sem_rx(uint32_t pl, uint32_t ph)
{
    uint32_t i;
    uint32_t ft;
    uint32_t gt;
    int32_t rx = 0;

    if (ph < 1) ph = 1;

    ft = pl/ph;
    gt = pl%ph;

    for (i = 1; i < NEL_SE_NX; i++) {
        if (NEL_SE_LRX[i] > ft) break;
    }

    if (i == NEL_SE_NX-1 && NEL_SE_LRX[NEL_SE_NX-1] <= ft) {
        if (ft < NEL_SE_B1) {
            rx = NEL_SE_X2-4;
        }
        else if (ft < NEL_SE_B2) {
            rx = NEL_SE_X2-3;
        }
        else if (ft < NEL_SE_B3) {
            rx = NEL_SE_X2-2;
        }
        else if (ft < NEL_SE_B4) {
            rx = NEL_SE_X2-1;
        }
        else {
            rx = NEL_SE_X2;
        }
    }
    else {
        rx = (i-1)*NEL_SE_TS+gt*NEL_SE_TS/ph;
    }

    rx = NEL_SE_X2+1-rx;

    if (rx < NEL_SE_X1) rx = NEL_SE_X1;
    if (rx > NEL_SE_X2) rx = NEL_SE_X2;

    return rx;
}


