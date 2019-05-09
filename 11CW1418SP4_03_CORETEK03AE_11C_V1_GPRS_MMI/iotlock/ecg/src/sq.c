/*
 * Signal Quality.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-23-2015;
 */

/**
 * @file sq.c
 * @brief Signal quality. Range from 1-5, with 5 being the best quality, 1 the worst;
 */

#include <stdlib.h>
#include <string.h>
#include "imath.h"
#include "util.h"
#include "sq.h"


#define NEL_SQ_MAX(a, b) ((a) > (b) ? (a) : (b))
#define NEL_SQ_MIN(a, b) ((a) > (b) ? (b) : (a))
#define NEL_SQ_ABS(a) ((a) > 0 ? (a) : (-(a)))


static const int32_t NEL_QA[][4] = {
        {900, 1200, 1600, 2200},
        {750, 1000, 1300, 1800},
        {650, 800, 1000, 1400},
        {450, 550, 700, 950}
};
static const int32_t NEL_QB[] = {450, 670, 780, 880};
static const int32_t NEL_QC[] = {380, 320, 260, 200};
static const int32_t NEL_QD[] = {32, 0, 128};


void nel_sq_init(nq_t* ss)
{
    memset(ss, 0, sizeof(nq_t));
}

static void nel_sq_norm(const int16_t* x, int16_t* t)
{
    int32_t sum = 0;
    int32_t std = 0;
    int32_t i;

    for (i = 0; i < NEL_NQ_N; i++) {
        t[i] = x[i];
        sum += t[i];
    }

    sum /= NEL_NQ_N;

    for (i = 0; i < NEL_NQ_N; i++) {
        t[i] -= sum;
        std += ((int32_t)t[i])*((int32_t)t[i])/(NEL_NQ_N-1);
    }
    std = (int32_t)nel_isqrt(std);

    if (std < 1) std = 1;

    for (i = 0; i < NEL_NQ_N; i++) {
        t[i] = ((int32_t)t[i])*2048/std;
    }
}

static int32_t nel_sq_pcc(const int16_t* x, const int16_t* y)
{
    int32_t i;
    int32_t r = 0;
    int16_t xt[NEL_NQ_N] = {0};
    int16_t yt[NEL_NQ_N] = {0};

    nel_sq_norm(x, xt);
    nel_sq_norm(y, yt);

    for (i = 0; i < NEL_NQ_N; i++) {
        r += ((int32_t)xt[i])*((int32_t)yt[i])/(NEL_NQ_N-1);
    }

    return r/4194;
}


static void nel_sq_update_overall(nq_t* ss)
{
    int32_t n = ss->nb-NEL_NQ_M;
    n = n < 1 ? 1 : n;
    ss->a1++;
    ss->a0 += ss->ql;
    if (ss->ql < 3 && n > 20) {
        ss->a1 += 2;
        ss->a0 += ss->ql*2; 
    }
    ss->lsq = (ss->a0*1000)/ss->a1;
    ss->hu = 1;
}

int16_t nel_sq_add_sample(nq_t* ss, const int16_t x, const int16_t rri, const uint8_t iz)
{
    int32_t i = 0;
    int16_t r0 = rri;

    ss->hu = 0;

    NEL_FIFO(ss->s0, x);

    ss->nn += iz > 0;

    if (ss->nb > NEL_NQ_M && ss->nn > NEL_QD[2]) {
        ss->ql = 1;
        nel_sq_update_overall(ss);
        ss->nn = 0;
        return ss->ql;
    }

    if (ss->ny > 0) {
        ss->ny--;
    }
    else if (ss->ny < 0) {
        ss->ny++;
    }

    if (r0 > 0) {
        ss->ny = -8;
        ss->r0 = r0;
    }

    if (ss->ny == -1) {
        int16_t mi = 0;
        int16_t mv = ss->s0[51];
        for (i = 1; i < 12; i++) {
            if (ss->s0[51+i] > mv) {
                mv = ss->s0[51+i];
                mi = i;
            }
        }
        ss->ny = 32+mi;
    }
    else if (ss->ny == 1) {
        int32_t q1 = 5;
        int32_t q2 = 5;
        int32_t q3 = 5;
        int32_t q4 = 5;
        int32_t dx = 0;
        int32_t mr = 0;
        int32_t mb = 0;

        if (ss->nb > NEL_NQ_M) {
            if (ss->nn > NEL_QD[0]) q2 = 1;
            else if (ss->nn > NEL_QD[1]) q2 = 2;

            ss->c2 = nel_sq_pcc(ss->s0, ss->s1);
            if (ss->c2 < NEL_QB[0]) q3 = 1;
            else if (ss->c2 < NEL_QB[1]) q3 = 2;
            else if (ss->c2 < NEL_QB[2]) q3 = 3;
            else if (ss->c2 < NEL_QB[3]) q3 = 4;

            ss->c3 = ((int32_t)(NEL_SQ_ABS((ss->r0-ss->r1)))*1000)/((int32_t)(NEL_SQ_MAX(ss->r1, 1)));
            if (ss->c3 > NEL_QC[0]) q1 = 1;
            else if (ss->c3 > NEL_QC[1]) q1 = 2;
            else if (ss->c3 > NEL_QC[2]) q1 = 3;
            else if (ss->c3 > NEL_QC[3]) q1 = 4;

            for (i = 11; i < 28; i++) {
                dx = NEL_SQ_ABS(ss->s0[i]-ss->s0[i-7]);
                mr = NEL_SQ_MAX(dx, mr);
            }
            for (i = 35; i < NEL_NQ_N; i++) {
                dx = NEL_SQ_ABS(ss->s0[i]-ss->s0[i-7]);
                mb = NEL_SQ_MAX(dx, mb);
            }
            dx = mr*1000/(mb == 0 ? 1 : mb);
            ss->c1 = dx;
            {
                int32_t k = 0;
                int32_t qk = NEL_SQ_MIN(q3, q1);
                if (qk == 3) k = 1;
                else if (qk == 4) k = 2;
                else if (qk == 5) k = 3;
                if (dx < NEL_QA[k][0]) q4 = 1;
                else if (dx < NEL_QA[k][1]) q4 = 2;
                else if (dx < NEL_QA[k][2]) q4 = 3;
                else if (dx < NEL_QA[k][3]) q4 = 4;
            }

            ss->ql = NEL_SQ_MIN(NEL_SQ_MIN(NEL_SQ_MIN(q1, q2), q3), q4);
            nel_sq_update_overall(ss);
        }

        if (ss->nb == 0) {
            memcpy(ss->s1, ss->s0, sizeof(ss->s0));
        }
        else if (ss->nb < NEL_NQ_M) {
            for (i = 0; i < NEL_NQ_N; i++) {
                ss->s1[i] = ss->s1[i]/2+ss->s0[i]/2;
            }
        }
        else {
            for (i = 0; i < NEL_NQ_N; i++) {
                ss->s1[i] = ss->s1[i]*3/4+ss->s0[i]/4;
            }
        }

        ss->nb++;
        ss->nn = 0;
        ss->r1 = ss->r0;
    }

    if (ss->nb <= NEL_NQ_M) {
        ss->ql = 0;
    }

    return ss->ql;
}

#undef NEL_SQ_MIN
#undef NEL_SQ_MAX
#undef NEL_SQ_ABS

