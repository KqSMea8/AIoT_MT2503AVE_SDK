/*
 * Mood value is the reverse of relaxation level.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-27-2015;
 */

#include "nsk_defines.h"
#include "spectr.h"
#include "mood.h"


void nel_mood_init(nm_t* ss)
{
    ss->pl = 0;
    ss->ph = 0;
    ss->nb = 0;
}


int16_t nel_mood_evaluate(nm_t* ss, const int16_t* rri, const int32_t n)
{
    uint32_t lf = 0;
    uint32_t hf = 0;

    if (n < 1) {
        ss->rm = -1;
        return ss->rm;
    }
    if (ss->nb == n) goto label_compute_mood;

    ss->nb = n;

    nel_sem_compute_hflf(rri, n, &lf, &hf);

    ss->pl += lf;
    ss->ph += hf;

    if (ss->pl > 2147483647 || ss->ph > 2147483647) {
        ss->pl >>= 1;
        ss->ph >>= 1;
    }

label_compute_mood:
    ss->rm = 101-nel_sem_rx(ss->pl, ss->ph);
    return ss->rm;
}


