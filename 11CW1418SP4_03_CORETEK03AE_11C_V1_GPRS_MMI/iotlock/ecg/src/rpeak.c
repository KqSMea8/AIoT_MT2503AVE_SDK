/*
 * Heart beat R-peak detector.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 04-05-2015;
 */

#include <string.h>
#include "nsk_defines.h"
#include "param.h"
#include "rpeak.h"

#define NEL_RD_M0 77
#define NEL_RD_M1 512
#define NEL_RD_RL 154
#define NEL_RD_RH 205
#define NEL_RD_RP 96
#define NEL_RD_RQ 320
#define NEL_RD_XW 768
#define NEL_RD_XU 6144
#define NEL_RD_XL 614
#define NEL_RD_XA 512
#define NEL_RD_X1 2
#define NEL_RD_X2 3
#define NEL_RD_X3 3
#define NEL_RD_X4 2
#define NEL_RD_SU 55
#define NEL_RD_SD 20
#define NEL_RD_S1 3
#define NEL_RD_S2 2
#define NEL_RD_MD 2
#define NEL_RD_R1 5
#define NEL_RD_R2 2
#define NEL_RD_R3 4
#define NEL_RD_R4 5
#define NEL_RD_RR 8
#define NEL_RD_R5 20
#define NEL_RD_R6 3
#define NEL_RD_R7 25


void nel_rpeak_init(nd_t* ss)
{
    memset(ss, 0x00, sizeof(nd_t));
    ss->i1 = 1;
    ss->dw = NEL_RPEAK_EH;
    ss->dr = NEL_RPEAK_EH;
    ss->mi = NEL_RD_M0;
}


int16_t nel_rpeak_add_sample(nd_t* ss, const int16_t x)
{
    int16_t rv = -1;
    int32_t di = 0;

    int32_t t1;
    int32_t t2;
    int32_t dx;

    dx = x-ss->buf[ss->idx];
    ss->buf[ss->idx] = x;
    ss->idx++;
    ss->idx &= NEL_ND_N-1;

    ss->nx += 1;

    if (ss->nx%NEL_RD_XA == 0) {
        if (ss->dr < ss->dw*NEL_RD_X1/NEL_RD_X2) {
            ss->dr = ss->dr*NEL_RD_X3/NEL_RD_X4;
        }
        else {
            ss->dr = ss->dw;
        }

        if (ss->dr > NEL_RD_XU) ss->dr = NEL_RD_XU;

        if (ss->dr < NEL_RD_XL) ss->dr = NEL_RD_XL;

        ss->dw = 0;
    }

    if (ss->dw < dx) ss->dw = dx;

    if (ss->nx < NEL_RD_XA) ss->dr = ss->dw;

    t1 = ss->dr*NEL_RD_SU/100;
    t2 = ss->dr*NEL_RD_SD/100;

    if (dx > t1) ss->iu = 1;

    if ((ss->iu == 1) && (dx < t2) && (ss->h2 == 0)) {
        ss->h1 = 1;
    }

    if ((ss->h1 == 1)
            && ((dx > t1)
            || (dx < -t1*NEL_RD_S1/NEL_RD_S2))) {
        ss->h1 = 0;
        ss->h2 = 1;

        if (ss->ni < ss->mi) {
            if (ss->nd < NEL_RD_MD) ss->ni = 0;

            di = -1;
        }
        else {
            di = ss->ni;
            ss->ni = 0;
        }
    }

    if ((ss->ni == 0) && (di > 0)) {
        if (ss->i1 == 1) {
            ss->i1 = 0;
        }
        else {
            rv = di;
        }

        ss->nd += 1;

        if (di > NEL_RD_XW) {
            ss->nd = 0;
            ss->ai = 0;
            ss->mi = NEL_RD_M0;
            di = -1;
            rv = -1;
        }

        if (ss->nd > NEL_RD_MD) {
            if (ss->ai == 0) {
                if ((di > NEL_RD_M0) && (di < NEL_RD_M1)) {
                    ss->ai = di;
                }
            }
            else {
                if ((di > NEL_RD_M0)
                        && (di < ss->ai*NEL_RD_R1/NEL_RD_R2)
                        && di < NEL_RD_M1)
                    ss->ai =
                            (di+ss->ai*NEL_RD_R3)/NEL_RD_R4;
            }

            if (((ss->nd > NEL_RD_RR)
                    && (ss->ai > NEL_RD_RP)
                    && (ss->ai < NEL_RD_RQ))
                    || ((ss->ai > NEL_RD_RL)
                    && (ss->ai < NEL_RD_RH)))
                ss->mi =
                        (ss->mi*NEL_RD_R5
                        +ss->ai*NEL_RD_R6)
                        /NEL_RD_R7;
        }
    }

    if (ss->h2 == 1) {
        ss->ni += 1;
        if (dx < t2) ss->h1 = 1;
    }

    if (rv != -1) {
        if (rv < NEL_RD_M0) rv = NEL_RD_M0;
        if (rv > NEL_RD_M1) rv = NEL_RD_M1;
    }
    return rv;
}

