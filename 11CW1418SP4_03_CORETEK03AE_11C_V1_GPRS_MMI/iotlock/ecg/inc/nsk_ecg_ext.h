/*
 * NeuroSky ECG extended algoirthm pipeline.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */

/**
 * @file nsk_ecg_ext.h
 * @brief NeuroSky ECG extended algoirthm pipeline, including Mood, HRV and Respiratory Rate calculation.
 * @details This header covers the extended algorithm pipeline for computing Mood, HRV and Respiratory Rate.
 * When compiled, this part of code requires about another 4 KB of code memory size, depending on the target platform.
 * Before using the ecg algorithm library, the API user needs to determine the sampling rate of incoming
 * ECG raw sample (usually 512 Hz from BMD101 chips), and set the sampling rate, and then initialize the pipeline.
 * Once initialized, the user then supply raw ECG sample by sample at the specified sampling rate, by calling the
 * update function. Meanwhile, the user may query various algorithm outputs, such as HR, Signal Quality, etc.
 * See below for a minimal example.
 * @code{.c}
 * nsk_ecg_set_raw_sample_rate(512); // 512 is the default, so it can be skipped if working with BMD100, BMD101 chips.
 * nsk_ecg_init(); // this can be used to reset too, if there is a lead-off to lead-on event.
 * nsk_ecg_update(raw_sample); // keep updating for every incoming raw ecg sample.
 * if (nsk_ecg_is_new_beat()) { // usually we only query algorithm outputs when a new beat is detected.
 *     rhr = nsk_ecg_get_hr_robust(); // Query the Robust Heart Rate.
 *     ssq = nsk_ecg_get_signal_quality(); // Query the short term Signal Quality.
 *     lsq = nsk_ecg_get_overall_signal_quality(); // Query the long term Signal Quality.
 *     if (nsk_ecg_get_rri_count() >= NEL_EXN) { // only query ext algorithms when enough RRI is ready.
 *         hrv = nsk_ecg_compute_hrv(); // only if HRV is needed.
 *         mood = nsk_ecg_compute_mood(); // only if Mood is needed.
 *         resp_rate = nsk_ecg_compute_respiratory_rate(); // only if Respiratory Rate is needed.
 *     }
 * }
 * @endcode
 */

#ifndef _WIN32
#pragma GCC visibility push(default)
#endif

#ifndef NSK_ECG_EXT_H
#define NSK_ECG_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nsk_defines.h"

/** The algorithms need this many RR Intervals (RRI) to start giving meaningful results. */
#define NEL_EXN 30

/**
 * Compute Mood Value. It requires that at least 30 RRI has been detected and stored.
 * Use @c nsk_ecg_get_rri_count() to check the amount of RRI currently in buffer.
 * A value of @b -1 will be returned if there has been less than 30 RRI detected.
 * In addition, the value will not change if no new RRI is detected.
 * Note that this function is relatively expensive, therefore we recommend only calling it sparsely,
 * For example, only call it for once when the data session has been finished, and @c nsk_ecg_get_rri_count() > 30;
 * The returned Mood value is on a scale of 1 to 100. The higher the number, the higher mood the user is at.
 */
int16_t
nsk_ecg_compute_mood(void);


/**
 * Compute HRV using robust statistics. It requires that at least 30 RRI
 * has been detected and stored. Use @c nsk_ecg_get_rri_count() to check the amount of RRI currently in buffer.
 * A value of @c -1 will be returned if there has been less than 30 RRI detected.
 * In addition, the value will not change if no new RRI is detected.
 * Note that this function is relatively expensive, therefore we recommend only calling it sparsely,
 * For example, only call it for once when the data session has been finished, and @c nsk_ecg_get_rri_count() > 30;
 * The return HRV value is in units of milliseconds, from 1 to 300.
 */
int16_t
nsk_ecg_compute_hrv(void);


/**
 * Compute Respiratory rate. It requires that at least 30 RRI
 * has been detected and stored. Use @c nsk_ecg_get_rri_count() to check the amount of RRI currently in buffer.
 * A value of @c -1 will be returned if there has been less than 30 RRI detected.
 * In addition, the value will not change if no new RRI is detected.
 * Note that this function is relatively expensive, therefore we recommend only calling it sparsely,
 * For example, only call it for once when the data session has been finished, and @c nsk_ecg_get_rri_count() > 30;
 * The return Respiratory rate value is in units of Breath-per-minute, from 6 to 40.
 */
int16_t
nsk_ecg_compute_respiratory_rate(void);


/**
 * Compute the Stress value. Stress is from 1-100, the larger the more stressful. It requires at least 30 RRI.
 * A value of @c -1 will be returned if there has been less than 30 RRI detected.
 * The user is responsible to mantain and provide an array for stress history data. This @c io_array
 * is a 16 bit integer array of length 128. At the very beginning when stress has never been calculated,
 * the user should supply an all-zero array. After the first call, this array is modified in-place everytime
 * it is called, and the user is responsible to save it to external storage after each call and load it
 * from external storage to supply as the last parameter whenever stress needs to be computed.
 */
int16_t
nsk_ecg_compute_stress(
    const uint8_t feedback, /**< user feedback for previous session, 1-100. Set to 0 if no feedback. */
    const uint8_t gender, /**< 0 for male, 1 for female. */
    const uint8_t age, /**< in years, 16-90. */
    const uint16_t height, /**< in cm, 1-300. */
    const uint16_t weight, /**< in kg, 1-300. */
    int16_t* io_array /**< An array of 16-bit integers of length 128. */
);


/**
 * Compute the Heart Age. The input age must be between 16-90 years, and the output HA is 16-105 years.
 * This algorithm requires at least 30 RRI. @c -1 will be returned if there has been less than 30 RRI detected.
 * @param[in] age the input user age.
 * @param[in, t] io_array An array of 16 bit integers of length 128. At the very beginning when
 * no history data is avaiable, supply a all-zero array. After the first call, this array is modified in-place everytime
 * it is called, and the user is responsible to save it to external storage after each call and load it
 * from external storage to supply as the last parameter whenever heart age needs to be computed.
 * @return the computed heart age, 16-105 years.
 * @note the io_array is of the same format as used in stress computation. Only one array is needed for both algorithms.
 */
int16_t
nsk_ecg_compute_heart_age(const uint8_t age, int16_t* io_array);

/**
 * Excercise is weight-efficient when it is performed within
 * a certain range of heart rates during exercise.
 * For achieving weight loss, exercise should be in the 2~3 
 * zones, and last more than 45 minutes of time (it takes
 * about 40 minutes to achieve glucose depletion and start
 * burning fat for energy), but preferably not more than an
 * hour.
 *
 * @return The current Heart Training Zone:
 *             -1 - invalid argument(s)
 *              0 - no training
 *              1 - entering minimal training zone; serves as warmup
 *                  before exercise or keeping in shape
 *              2 - fat burning zone (aerobic exercise); achieving
 *                  general fitness and fat burn
 *              3 - glycogen consumption zone; training lung function
 *                  and endurance
 *              4 - anaerobic training zone; for boosting fitness,
 *                  speed, and power
 *              5 - maximum zone; at the typical limits of human
 *                  high-intensity exercise
 */
int8_t
nsk_ecg_get_heart_training_zone( 
    const int8_t gender,   /* 0 for male, 1 for female */
    const int8_t age,      /* in years, 16-90 */
    const int16_t weight,  /* in kg, 1-300 */
    const int16_t hr       /* in bpm, 30-220 */
);

#ifdef __cplusplus
}
#endif

#endif

#ifndef _WIN32
#pragma GCC visibility pop
#endif

