/*
 * Median of an array.
 */

/**
 * @file median.h
 * @brief Quickselect median from an array;
 */

#ifndef MEDIAN_H
#define MEDIAN_H

#include "nsk_defines.h"

/** Median of an int16_t array of length n. */
int16_t
nel_quick_median(int16_t arr[], int32_t n);

/** Median of an int32_t array of length n. */
int32_t
quick_select_median(int32_t arr[], int32_t n);

#endif

