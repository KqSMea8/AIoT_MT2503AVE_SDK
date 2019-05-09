/*
 * Preprocessing filter, Band pass FIR filter.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

#include <string.h>
#include "prep.h"

#define NEL_NP_D 14
#define NEL_NP_S 16384


#if (defined NEL_USE_DIVISION_FOR_RSHIFT && NEL_USE_DIVISION_FOR_RSHIFT == 1)
    #define NEL_NP_SC(a) ((a)/NEL_NP_S)
#else
    #define NEL_NP_SC(a) ((a)>>NEL_NP_D)
#endif


static const int16_t NEL_NP_PRC[2*NEL_NP_N] = {
    5,13,9,-31,-122,-248,-349,-356,-253,-114,-75,-224,-501,-705,-639,-304,35,13,-513,-1249,
    -1557,-877,819,2872,4271,4271,2872,819,-877,-1557,-1249,-513,13,35,-304,-639,-705,-501,
    -224,-75,-114,-253,-356,-349,-248,-122,-31,9,13,5,
    5,13,9,-31,-122,-248,-349,-356,-253,-114,-75,-224,-501,-705,-639,-304,35,13,-513,-1249,
    -1557,-877,819,2872,4271,4271,2872,819,-877,-1557,-1249,-513,13,35,-304,-639,-705,-501,
    -224,-75,-114,-253,-356,-349,-248,-122,-31,9,13,5
};

void nel_prep_init(np_t* ss)
{
    memset(ss, 0, sizeof(np_t));
}


int16_t nel_prep_add_sample(np_t* ss, const int16_t x)
{
    int32_t sum;
    int32_t i;
    int32_t i0;
    const int16_t* pc;
    const int16_t* pb;

    i0 = ss->il;
    ss->buf[i0] = x;
    pc = NEL_NP_PRC+NEL_NP_N-i0;
    pb = ss->buf;
    sum = 0;

    for (i = 0; i < NEL_NP_N; i++) {
        sum += ((int32_t)(*(pc++)))*((int32_t)(*(pb++)));
    }

    if (--i0 < 0) i0 += NEL_NP_N;
    ss->il = i0;
    return (int16_t)(NEL_NP_SC(sum));
}


