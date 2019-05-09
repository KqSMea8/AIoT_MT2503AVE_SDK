/*
 * Heart Age.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 07-13-2015;
 */

#include <stdlib.h>
#include <string.h>
#include "ha.h"
#include "io_array.h"

#define NEL_HAG 15
#define NEL_HAH 15
#define NEL_HAI 8
#define NEL_HAK 11
#define NEL_HAN 15

static int32_t cmp (const void *pa, const void *pb)
{
    const int16_t (*aa)[2] = (const int16_t (*)[2])pa;
    const int16_t (*bb)[2] = (const int16_t (*)[2])pb;
    return (*aa)[0]-(*bb)[0];
}

uint8_t nel_heart_age(const uint8_t age, const int16_t hrv, const int16_t rrc, int16_t* io_array)
{
    const uint8_t h1[NEL_HAI] = {86, 66, 47, 36, 31, 28, 20, 1};
    const uint8_t h2[NEL_HAI] = {16, 20, 30, 40, 50, 60, 90, 105};
    const uint8_t h3[NEL_HAK] = {21, 42, 52, 60, 67, 74, 80, 85, 90, 95, 100};

    int16_t (*o)[NEL_IOE] = (int16_t (*)[NEL_IOE])io_array;
    int16_t g = o[NEL_IOH][NEL_IOZ];
    int16_t a[NEL_IOE][2] = {0};

    int32_t i;
    int32_t k;
    int32_t total_wei = 0;
    int32_t q3_hrv = 0;
    int32_t hd = age;
    int32_t hb;
    int32_t hc;
    int32_t ha;

    if (g > NEL_IOE || hrv < 1 || hrv > 300 || rrc < 1 || io_array == NULL) return age;
    if (hd < h2[0]) hd = h2[0];
    if (hd > h2[NEL_HAI-2]) hd = h2[NEL_HAI-2];
    o[NEL_IOH][NEL_IOB] = age;

    if (g < NEL_IOE) {
        o[NEL_IOM][g] = hrv;
        o[NEL_ION][g] = rrc;
        g++;
        o[NEL_IOH][NEL_IOZ] = g;
    }
    else if (g == NEL_IOE) {
        for (i = 0; i < NEL_IOE-1; i++) {
            o[NEL_IOM][i] = o[NEL_IOM][i+1];
            o[NEL_ION][i] = o[NEL_ION][i+1];
        }
        o[NEL_IOM][NEL_IOE-1] = hrv;
        o[NEL_ION][NEL_IOE-1] = rrc;
    }

    for (i = 0; i < g; i++) {
        k = (o[NEL_ION][i]/NEL_HAN)-1;
        if (k < 0) k = 0;
        if (k > NEL_HAK-1) k = NEL_HAK-1;
        a[i][0] = o[NEL_IOM][i];
        a[i][1] = h3[k];
        total_wei += a[i][1];
    }

    qsort(a, g, sizeof(a[0]), cmp);

    k = 0;
    for (i = 0; i < g; i++) {
        k += a[i][1];
        if (k > total_wei/2) break;
    }

    if (g < 2) {
        q3_hrv = a[0][0];
    }
    else {
        q3_hrv = (a[i][0]+a[i/2][0])/2;
    }

    if (q3_hrv >= h1[0]) {
        hb = h2[0];
    }
    else if (q3_hrv <= h1[NEL_HAI-1]) {
        hb = h2[NEL_HAI-1];
    }
    else {
        for (i = 1; i < NEL_HAI; i++) {
            if (h1[i] == q3_hrv) {
                hb = h2[i];
                break;
            }
            else if (h1[i] < q3_hrv) {
                break;
            }
        }
        hb = (q3_hrv-(int32_t)h1[i-1])*((int32_t)h2[i]-(int32_t)h2[i-1])
                /((int32_t)h1[i]-(int32_t)h1[i-1])+(int32_t)h2[i-1];
    }
    hc = hb-hd;

    if (hc == 0) {
        ha = hb;
    }
    else if (hc > 0) {
        ha = hc*NEL_HAG/((int32_t)h2[NEL_HAI-1]-hd)+hd;
    }
    else {
        ha =  hc*NEL_HAH/hd+hd;
    }

    if (ha < h2[0]) ha = h2[0];
    if (ha > h2[NEL_HAI-1]) ha = h2[NEL_HAI-1];

    return ha;
}



