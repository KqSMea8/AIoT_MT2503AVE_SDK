/*
 * ECG lib parameters.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-20-2015;
 */

/**
 * @file param.h
 * @brief Set some gobal definition of constants related to ECG algorithms.
 */

#ifndef PARAM_H
#define PARAM_H


/** Analog digital conversion units per millivolt, used in BMD100, BMD101, and BMD200. */
#define NEL_ADC 6990

/** Sampling rate internally used by the algorithm library */
#define NEL_FS 256

/** Bit depth of the above quantity */
#define NEL_FS_BIT_DEPTH 8

/** We expect to see the height of a normal R-peak at this value. */ 
#define NEL_RPEAK_EH 3072


#endif

