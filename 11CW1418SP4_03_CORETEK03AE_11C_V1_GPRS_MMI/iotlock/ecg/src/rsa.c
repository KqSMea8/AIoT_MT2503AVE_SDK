/*
 * Respiratory rate.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 4-15-2015;
 */

#include <stdlib.h>
#include <string.h>

#include "nsk_defines.h"
#include "rsa.h"


#define NEL_RS_SL 360
#define NEL_RS_SU 6000


void nel_rsa_init(na_t* ss)
{
    memset(ss, 0, sizeof(na_t));
}


int32_t nel_rsa_add_sample(na_t* ss, const int32_t x)
{
    int8_t d;
    int32_t r = 0;

    ss->xc++;

    if (ss->xc == 1) {
        ss->nt += x;
        ss->x0 = x;
        return r;
    }

    d = x > ss->x0 ? 1 : (x == ss->x0 ? 0 : -1);

    if (ss->pv == 0) {
        ss->pv = d;
    }
    else if ((ss->pv > 0 && d < 0) || (ss->pv < 0 && d > 0)) {
        int32_t t = ss->nt-(ss->cf/2*ss->x0)-ss->lk;
        r = ss->xc-(ss->cf/2)-1;

        if (t >= NEL_RS_SL && t <= NEL_RS_SU) {
            if (d < 0) ss->np++;
            else ss->nv++;
        }

        if (d > 0) r = -r;

        ss->cf = 0;
        ss->lk = ss->nt;
    }
    else if (d != 0) {
        ss->cf = 0;
    }

    if (d == 0) {
        ss->cf++;
    }
    else {
        ss->pv = d;
    }

    ss->nt += x;
    ss->x0 = x;
    return r;
}


int32_t nel_rsa_compute(na_t* ss)
{
    int32_t r = (ss->np > ss->nv ? ss->nv : ss->np)*60000/(ss->nt+1);
    if (r < NEL_RS_L) r = NEL_RS_L;
    if (r > NEL_RS_U) r = NEL_RS_U;
    return r;
}


