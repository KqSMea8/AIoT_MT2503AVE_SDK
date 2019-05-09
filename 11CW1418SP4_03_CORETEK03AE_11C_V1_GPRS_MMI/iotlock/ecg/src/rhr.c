/*
 * Robust Heart Rate.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 2-25-2015;
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nsk_defines.h"
#include "rhr.h"


void nel_rhr_init(nh_t* ss, uint8_t th)
{
    memset(ss, 0, sizeof(nh_t));
    ss->th = th;
}


void nel_rhr_update(nh_t* ss, int16_t sq, int16_t hr, int32_t c)
{
    int32_t t, w;
    if (hr < 1) return;

    if (ss->r0 < 1) t = abs(c%36-18)+64;
    else t = ss->r0;

    if (hr > 42 && hr < 144) {
        if (sq < ss->th) {
            w = -(t*7+hr)/8;
        }
        else {
            if (ss->r0 < 1 && hr >= 62 && hr <= 96) {
                w = (t+hr)/2;
            }
            else {
                w = (t+hr*3)/4;
            }
            ss->r0 = w;
        }
    }
    else {
        if (sq == 5) {
            w = (t*15+hr)/16;
            ss->r0 = w;
        }
        else if (sq < ss->th) {
            w = -t;
        }
        else {
            w = -(t*15+hr)/16;
        }
    }
    ss->r1 = w;
}


