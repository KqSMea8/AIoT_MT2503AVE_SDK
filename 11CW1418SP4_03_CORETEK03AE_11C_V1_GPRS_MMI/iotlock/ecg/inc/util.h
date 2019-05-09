/*
 * NeuroSky ECG library utilities.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-16-2015;
 */

/**
 * @file util.h
 * @brief NeuroSky ECG Library utilities.
 */

#ifndef NEL_UTIL_H
#define NEL_UTIL_H


/** Operate on a first in first t buffer defined by @c buf, add new element x to be tail. */
#define NEL_FIFO(buf, x) do {memmove(&(buf[0]), &(buf[1]), (sizeof(buf)-sizeof(buf[0]))); \
    (buf[(sizeof(buf)/sizeof(buf[0]))-1]) = (x);} while (0)


#endif

