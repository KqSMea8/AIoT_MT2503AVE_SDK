/*
 * Noise checker.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

#include <string.h>
#include <stdlib.h>
#include "nsk_defines.h"
#include "param.h"
#include "noise.h"


#define NEL_NN_N1 8
#define NEL_NN_N2 48
#define NEL_NN_N3 8
#define NEL_NN_NC 6
#define NEL_NN_TA 0x01
#define NEL_NN_TB 0x02
#define NEL_NN_TC 0x04
#define NEL_NN_TD 0x08
#define NEL_NN_TE 0x10
#define NEL_NN_TF 0x20
#define NEL_NN_T1 16384
#define NEL_NN_T2 4800
#define NEL_NN_T3 1500
#define NEL_NN_T4 5
#define NEL_NN_T5 18
#define NEL_NN_T6 10
#define NEL_NN_T7 22


void nel_noise_init(nn_t* ss)
{
    int32_t i;
    memset(ss, 0, sizeof(nn_t));

    for (i = 0; i < NEL_NN_LX; i++) {
        ss->br[i] = NEL_NN_TF;
    }

    ss->t1 = NEL_NN_T1;
    ss->t2 = NEL_NN_T2;
    ss->t3 = NEL_NN_T3;
}


int16_t nel_noise_add_sample(nn_t* ss, const int16_t x)
{
    int16_t nv = NEL_NN_TF;
    int32_t x0;
    int32_t x1;
    int32_t i;
    int32_t sum = 0;
    uint8_t c20;
    uint8_t c21;
    uint8_t c30;
    uint8_t c31;

    ss->nx++;

    ss->xd = ss->bx[ss->ix];
    ss->bx[ss->ix] = x;
    ss->ix++;
    ss->ix &= NEL_NN_LX-1;

    if (ss->nx < NEL_NN_N1+NEL_NN_N2+2) return nv;

    x0 = abs(ss->bx[(ss->ix+NEL_NN_N1+NEL_NN_N2-1)&(NEL_NN_LX-1)]);
    x1 = abs(ss->bx[(ss->ix+NEL_NN_N1+NEL_NN_N2)&(NEL_NN_LX-1)]);
    
    if (x1 > ss->t1) {
        for (i = 0; i < NEL_NN_LX; i++) {
            ss->br[i] |= NEL_NN_TE;
        }
    }

    c31 = x1 > ss->t3;
    c30 = x0 <= ss->t3;
    c21 = x1 > ss->t2;
    c20 = x0 <= ss->t2;

    if (c31 && c30) ss->np3++;
    if (!c31 && !c30) ss->np3++;
    if (c21) ss->no2++;
    if (c21 && c20) ss->np2++;
    if (!c21 && !c20) ss->np2++;

    if (x1 == 0) {
        ss->nz += 1;
        if (ss->nz > NEL_NN_T7) {
            for (i = 0; i < NEL_NN_LX; i++) {
                ss->br[i] |= NEL_NN_TA;
            }
            ss->nz = 0;
        }
    }
    else { 
        if (ss->nz > 0) ss->nz = 0;
    }

    ss->ns++;

    if (ss->ns == NEL_NN_NC) {
        ss->bp2[ss->ip2] = ss->np2;
        ss->ip2++;
        ss->ip2 &= NEL_NN_LW-1;

        ss->bp3[ss->ip3] = ss->np3;
        ss->ip3++;
        ss->ip3 &= NEL_NN_LW-1;

        ss->bo2[ss->io2] = ss->no2;
        ss->io2++;
        ss->io2 &= NEL_NN_LW-1;

        sum = 0;
        for (i = 0; i < NEL_NN_LW; i++) {
            sum += ss->bp2[i];
        }
        
        if (sum > NEL_NN_T4) {
            for (i = 0; i < NEL_NN_LX; i++) {
                ss->br[i] |= NEL_NN_TD;
            }
        }

        sum = 0;
        for (i = 0; i < NEL_NN_LW; i++) {
            sum += ss->bo2[i];
        }

        if (sum > NEL_NN_T5) {
            for (i = 0; i < NEL_NN_LX; i++) {
                ss->br[i] |= NEL_NN_TC;
            }
        }

        sum = 0;
        for (i = 0; i < NEL_NN_LW; i++) {
            sum += ss->bp3[i];
        }

        if (sum > NEL_NN_T6) {
            for (i = 0; i < NEL_NN_LX; i++) {
                ss->br[i] |= NEL_NN_TB;
            }
        }

        ss->np2 = 0;
        ss->no2 = 0;
        ss->np3 = 0;
        ss->ns = 0;
    }

    nv = ss->br[ss->ir];
    ss->br[ss->ir] = 0;
    ss->ir++;
    ss->ir &= NEL_NN_LX-1;

    return nv;
}

