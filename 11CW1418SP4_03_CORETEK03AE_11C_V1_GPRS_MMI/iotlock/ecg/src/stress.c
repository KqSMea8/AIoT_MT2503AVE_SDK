/*
 * Stress algorithm.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 08-05-2015;
 */

#include <stdlib.h>
#include <string.h>
#include "median.h"
#include "stress.h"
#include "rsa.h"
#include "io_array.h"

#define NEL_STK 833
#define NEL_STM 5
#define NEL_STO 100
#define NEL_STN 16

static int32_t nel_st_cmp(const void *a, const void *b)
{
    return *(int16_t*)a-*(int16_t*)b;
}

int32_t nel_stress(
    const uint8_t feedback,
    const uint8_t gender,
    const uint8_t age,
    const uint16_t height,
    const uint16_t weight,
    const int16_t* rri,
    int16_t* io_array)
{
    const int16_t c[NEL_STM] = {9742, -68, -93, 125, -1421};
    const int32_t d[NEL_STO] = {
        -99999,-43695,-36636,-32452,-29445,-27081,-25124,-23446,-21973,-20655,-19460,-18363,-17347,-16398,-15506,-14664,
        -13863,-13100,-12368,-11665,-10987,-10331,-9695,-9076,-8473,-7885,-7309,-6745,-6191,-5646,-5109,-4579,-4055,
        -3537,-3023,-2514,-2007,-1503,-1001,-501,0,333,666,1000,1335,1670,2006,2344,2682,3022,3364,3708,4054,4403,
        4754,5108,5465,5826,6190,6558,6931,7308,7691,8079,8472,8873,9279,9694,10116,10546,10986,11435,11895,12367,
        12851,13350,13862,14392,14939,15505,16094,16706,17346,18015,18718,19459,20243,21078,21972,22934,23978,
        25123,26390,27814,29444,31354,33672,36635,40775,50000};

    int16_t (*o)[NEL_IOE] = (int16_t (*)[NEL_IOE])io_array;
    int16_t g = o[NEL_IOH][NEL_IOP];
    int16_t a[NEL_ST_RRI_LEN] = {0};
    int32_t i = 0;
    int32_t x = 0;
    int32_t p = NEL_STK;
    int32_t lt = o[NEL_IOH][NEL_IOX];
    int32_t t = -1;
    na_t rsa;

    if (g < 0) return t;
    if (g > 0) {
        if (g < 32) {
            x = g < NEL_STN ? g : NEL_STN;
            qsort(o[NEL_IOU], x, sizeof(int16_t), nel_st_cmp);
            p = o[NEL_IOU][(x+1)/2-1];
        }
        else {
            memcpy(a, o[NEL_IOU], sizeof(int16_t)*NEL_STN);
            p = nel_quick_median(a, NEL_STN);
        }
    }

    memcpy(a, rri, sizeof(int16_t)*NEL_ST_RRI_LEN);
    qsort(a, NEL_ST_RRI_LEN, sizeof(int16_t), nel_st_cmp);

    if (g < NEL_STN) {
        *(o[NEL_IOU]+g) = a[14];
    }
    else {
        for (i = 0; i < NEL_STN-1; i++) *(o[NEL_IOU]+i) = *(o[NEL_IOU]+i+1);
        *(o[NEL_IOU]+NEL_STN-1) = a[14];
    }
    o[NEL_IOH][NEL_IOP] = g+1;

    t = c[0]; 
    x = a[14]-p;
    if (x > 300) x = 300;
    if (x < -300) x = -300;
    t += c[1]*x;
    x = a[26]-a[3];
    if (x > 600) x = 600;
    if (x < 0) x = 0;
    t += c[2]*x;
    x = a[23]-a[6];
    if (x > 300) x = 300;
    if (x < 0) x = 0;
    t += c[3]*x;
    nel_rsa_init(&rsa);
    for (i = 0; i < NEL_ST_RRI_LEN; i++) nel_rsa_add_sample(&rsa, rri[i]);
    x = rsa.np;
    if (x > 13) x = 13;
    if (x < 1) x = 1;
    t += c[4]*x;
    i = 0;
    while (i < NEL_STO && d[i] < t) i++;
    t = i;

    if (g == 0) {
        t = (40+t)/2;
    }
    else if (g < NEL_STN) {
        t = lt+(t-lt)*(g+1)/NEL_STN;
    }

    if (t < 1) t = 1;
    if (t > 100) t = 100;

    o[NEL_IOH][NEL_IOA] = gender;
    o[NEL_IOH][NEL_IOB] = age;
    o[NEL_IOH][NEL_IOD] = height;
    o[NEL_IOH][NEL_IOS] = weight;
    o[NEL_IOH][NEL_IOT] = feedback;
    o[NEL_IOH][NEL_IOX] = t;
    return t;
}

