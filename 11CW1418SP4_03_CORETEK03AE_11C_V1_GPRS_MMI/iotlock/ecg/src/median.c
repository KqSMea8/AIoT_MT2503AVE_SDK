/* 
 * median of an array.
 */

/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */

#include <stdlib.h>
#include <string.h>
#include "nsk_defines.h"
#include "median.h"

#define NEL_MED_SW(a,b) { register int16_t t=(a);(a)=(b);(b)=t; }

int16_t nel_quick_median(int16_t arr[], int32_t n)
{
    int32_t low;
    int32_t high;
    int32_t median;
    int32_t middle;
    int32_t ll;
    int32_t hh;

    low = 0; high = n-1; median = (low + high) / 2;
    for (;;) {
        if (high <= low)
            return arr[median];

        if (high == low + 1) {
            if (arr[low] > arr[high])
                NEL_MED_SW(arr[low], arr[high]);
            return arr[median];
        }

        middle = (low + high) / 2;
        if (arr[middle] > arr[high])    NEL_MED_SW(arr[middle], arr[high]);
        if (arr[low] > arr[high])       NEL_MED_SW(arr[low], arr[high]);
        if (arr[middle] > arr[low])     NEL_MED_SW(arr[middle], arr[low]);

        NEL_MED_SW(arr[middle], arr[low+1]);

        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (arr[low] > arr[ll]);
            do hh--; while (arr[hh]  > arr[low]);

            if (hh < ll)
                break;

            NEL_MED_SW(arr[ll], arr[hh]);
        }

        NEL_MED_SW(arr[low], arr[hh]);

        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
}

#undef NEL_MED_SW


#define MED_SW(a,b) { register int32_t t=(a);(a)=(b);(b)=t; }
int32_t quick_select_median(int32_t arr[], int32_t n)
{
    int32_t low, high;
    int32_t median;
    int32_t middle, ll, hh;

    low = 0; high = n-1; median = (low + high) / 2;
    for (;;) {
        if (high <= low)
            return arr[median];

        if (high == low + 1) {
            if (arr[low] > arr[high])
                MED_SW(arr[low], arr[high]);
            return arr[median];
        }
        middle = (low + high) / 2;
        if (arr[middle] > arr[high])    MED_SW(arr[middle], arr[high]);
        if (arr[low] > arr[high])       MED_SW(arr[low], arr[high]);
        if (arr[middle] > arr[low])     MED_SW(arr[middle], arr[low]);
        MED_SW(arr[middle], arr[low+1]);
        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (arr[low] > arr[ll]);
            do hh--; while (arr[hh]  > arr[low]);

            if (hh < ll)
                break;

            MED_SW(arr[ll], arr[hh]);
        }
        MED_SW(arr[low], arr[hh]);
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
}



