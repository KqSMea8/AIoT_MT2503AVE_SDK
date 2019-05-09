/*
 * Robust HRV estimator.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-26-2015;
 */

#include <stdlib.h>
#include <string.h>
#include "nsk_defines.h"
#include "param.h"
#include "hrv.h"


static int32_t nel_hrv_cmp(const void* a, const void* b)
{
    return (*(int16_t*)a-*(int16_t*)b);
}


#define NEL_HRV_SW(a,b) { register int16_t t=(a);(a)=(b);(b)=t; }
static int16_t nel_hrv_kth_smallest(int16_t* a, int32_t n, int32_t k)
{
    int32_t i;
    int32_t j;
    int32_t l;
    int32_t m;
    int16_t x;

    l = 0;
    m = n-1;
    while (l < m) {
        x = a[k];
        i = l;
        j = m;
        do {
            while (a[i] < x) i++;
            while (x < a[j]) j--;
            if (i <= j) {
                NEL_HRV_SW(a[i],a[j]);
                i++;
                j--;
            }
        }
        while (i <= j);
        if (j < k) l = i;
        if (k < i) m = j;
    }
    return a[k];
}
#undef NEL_HRV_SW


static int16_t nel_hrv_median_lower(int16_t* x, int32_t n)
{
    return nel_hrv_kth_smallest(x, n, (n+1)/2-1);
}


int16_t nel_hrv_rescale(int16_t in)
{
    int32_t hrv = (int32_t)in*1000/NEL_FS;
    hrv = hrv > 2 ? (hrv*94+300)/100 : 1;

    if (hrv < NEL_HRV_L) hrv = NEL_HRV_L;
    if (hrv > NEL_HRV_U) hrv = NEL_HRV_U;

    return hrv;
}


int16_t nel_hrv_kernel(const int16_t* x, const int32_t n, int16_t* z, int16_t* y)
{
    int32_t la;
    int32_t lb;
    int32_t ra;
    int32_t ta;
    int32_t tb;
    int32_t diff;
    int32_t amin;
    int32_t amax;
    int32_t even;
    int32_t half;
    int32_t na;
    int32_t nb;
    int32_t length;
    /* int32_t rb; */
    int32_t i;
    int32_t ma;
    int32_t mb;
    int32_t cn;
    int32_t sn;
    if (n < 2) return 1;

    memcpy(y, x, n*sizeof(int16_t));
    qsort(y, n, sizeof(int16_t), nel_hrv_cmp);

    z[1-1] = y[n/2+1-1]-y[1-1];

    for (i = 2; i <= (n+1)/2; i++) {
        na = i-1;
        nb = n-i;
        diff = nb-na;
        la = 1;
        lb = 1;
        ra = nb;
        /* rb = nb; */
        amin = diff/2+1;
        amax = diff/2+na;
        while (la < ra) {
            length = ra-la+1;
            even = 1-(length%2);
            half = (length-1)/2;
            ta = la+half;
            tb = lb+half;
            if (ta < amin) {
                /* rb = tb; */
                la = ta+even;
            }
            else if (ta > amax) {
                ra = ta;
                lb = tb+even;
            }
            else {
                ma = y[i-1]-y[i-ta+amin-1-1];
                mb = y[tb+i-1]-y[i-1];
                if (ma >= mb) {
                    ra = ta;
                    lb = tb+even;
                }
                else {
                    /* rb = tb; */
                    la = ta+even;
                }
            }
        }

        if (la > amax) {
            z[i-1] = y[lb+i-1]-y[i-1];
        }
        else {
            ma = y[i-1]-y[i-la+amin-1-1];
            mb = y[lb+i-1]-y[i-1];
            z[i-1] = ma < mb ? ma : mb;
        }
    }

    for (i = (n+1)/2+1; i <= n-1; i++) {
        na = n-i;
        nb = i-1;
        diff = nb-na;
        la = 1;
        lb = 1;
        ra = nb;
        /* rb = nb; */
        amin = diff/2+1;
        amax = diff/2+na;
        while (la < ra) {
            length = ra-la+1;
            even = 1-(length%2);
            half = (length-1)/2;
            ta = la+half;
            tb = lb+half;
            if (ta < amin) {
                /* rb = tb; */
                la = ta+even;
            }
            else if (ta > amax) {
                ra = ta;
                lb = tb+even;
            }
            else {
                ma = y[i+ta-amin+1-1]-y[i-1];
                mb = y[i-1]-y[i-tb-1];
                if (ma >= mb) {
                    ra = ta;
                    lb = tb+even;
                }
                else {
                    /* rb = tb; */
                    la = ta+even;
                }
            }
        }

        if (la > amax) {
            z[i-1] = y[i-1]-y[i-lb-1];
        }
        else {
            ma = y[i+la-amin+1-1]-y[i-1];
            mb = y[i-1]-y[i-lb-1];
            z[i-1] = ma < mb ? ma : mb;
        }
    }

    z[n-1] = y[n-1]-y[(n+1)/2-1];
    cn = 100;
    if (n <= 9) {
        if (n == 2) cn = 74;
        if (n == 3) cn = 185;
        if (n == 4) cn = 95;
        if (n == 5) cn = 135;
        if (n == 6) cn = 99;
        if (n == 7) cn = 119;
        if (n == 8) cn = 100;
        if (n == 9) cn = 113;
    }
    else if ((n%2) == 1) {
        cn = n*1000/(n*10-9);
    }

    sn = cn*119*(int32_t)nel_hrv_median_lower(z, n)/10000;

    return nel_hrv_rescale(sn);
}


