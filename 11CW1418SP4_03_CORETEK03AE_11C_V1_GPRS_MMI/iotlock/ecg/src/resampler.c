/*
 * Raw ECG resampler.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015; 04-21-2015;
 */

#include <string.h>
#include "param.h"
#include "resampler.h"


void nel_resampler_init(nr_t* s, const uint16_t fs, const uint16_t ofs)
{
    memset(s, 0, sizeof(nr_t));
    s->fs = fs;
    s->ofs = ofs;
}


int8_t nel_resampler_add_sample(nr_t* s, const int32_t x)
{
    int8_t r = 0;
    if (s->fs == s->ofs<<1) {
        s->a2++;
        if (s->a2&1) {
            s->y[0] = x;
            r = 1;
        }
    }
    else if (s->fs == s->ofs) {
        s->y[0] = x;
        r = 1;
    }
    else if (s->fs == s->ofs/2) {
        s->y[0] = (s->x+x)/2;
        s->y[1] = x;
        s->x = x;
        r = 2;
    }
    else {
        while (s->a2 >= s->a1) {
            s->y[r] = x+((int32_t)(s->a2-s->a1)*(s->x-x))/(int32_t)s->ofs;
            if (s->a2 == s->a1) {
                s->a1 = 0;
                s->a2 = 0;
            }
            s->a1 += s->fs;
            r++;
        }
        s->a2 += s->ofs;
        s->x = x;
    }
    return r;
}


