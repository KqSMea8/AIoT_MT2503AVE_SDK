/*
 * NeuroSky ECG core algoirthm pipeline.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 03-11-2015;
 */

/**
 * @file nsk_ecg_core.h
 * @brief NeuroSky ECG core algoirthm pipeline, including a rescaler, a resampler, a preprocessing filter,
 * a noise checker, a r-peak detector, a RRI smoother, and a robust heart rate estimator.
 * @details This header covers the pipeline to detect R-peak, compute HR and robust HR, and estimate signal quality.
 * Depends on target platform, it requires about 4.8 KB of code memory size and 2.5 KB of read-write memory size
 * after compilation. The functionality to compute HRV, Mood and Respiratory Rate is covered by @c nsk_ecg_ext.h.
 * Check the detail documentation of @c nsk_ecg_ext.h for usage examples.
 */

#ifndef _WIN32
#pragma GCC visibility push(default)
#endif

#ifndef NSK_ECG_CORE_H
#define NSK_ECG_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nsk_defines.h"


/** A c-string about the version of this library. */
char*
nsk_ecg_get_lib_version_string(void);


/**
 * Rescale the input raw sample.
 * @param[in] x the input raw ECG sample. The raw value must be within the limit of a @b short integer,
 *            i.e. inclusively between -32768 to +32767.
 * @param[in] adc the Analog-digital conversion unit per 1 millivolt for the input data @c x.
 *            This value must be inclusively between 1 to 65535.
 * @return the rescaled ECG sample suitable for use by this libraray.
 * @note the raw ECG samples from NeuroSky BMD100, BMD101 and BMD200 can be used directly, rescale is not needed.
 */
int16_t
nsk_ecg_rescale_adc(const int16_t x, const int32_t adc);


/**
 * Set the sampling rate of the input raw data. This function should be called before calling @c nsk_ecg_init();
 * @param[in] fs the sampling rate of the raw ECG. Must be inclusively between 128 to 5120;
 *            The default sampling rate is 512 Hz, which is the native rate for NeuroSky BMD100 and BMD101 ECG.
 */
void
nsk_ecg_set_raw_sample_rate(const uint16_t fs);


/**
 * Initialize the ECG processing pipeline. Remember to set the sampling rate before initializing.
 * Also call this function if the ECG pipeline needs to be reset.
 */
void
nsk_ecg_init(void);


/**
 * Feed in one new raw ECG sample @c x, and update computations in the pipeline.
 * @param[in] x the input raw ECG sample. The raw value must be within the limit of a @b short integer,
 *            i.e. inclusively between -32768 to +32767.
 */
void
nsk_ecg_update(const int16_t x);


/** Returns 0 or 1; if 1 is returned, the R-peak of a new heart beat is just detected. */
uint8_t
nsk_ecg_is_new_beat(void);


/**
 * Get the short term signal quality, a value from 1 to 5, with 1 being the worst and 5 the best signal quality.
 * The return value is 0 when the signal quality algorithm is initializing and yet to settle (usually 1-2 seconds).
 * The short term signal quality is a diagnose of the usability of the current ECG signal.
 */
int16_t
nsk_ecg_get_signal_quality(void);


/**
 * Get the long term signal quality of the overall ECG. a value from 1000 to 5000, with 1000 being the worst and
 * 5000 the best. The value is 0 when the signal quality algorithm is initializing and yet to settle
 * (usually 1-2 seconds). The long term signal quality is an estimation of the validity of current ECG session.
 */
int16_t
nsk_ecg_get_overall_signal_quality(void);


/** Get the most recently detected R-to-R interval estimation, in milliseconds. Range is 300-2000 ms. */
int16_t
nsk_ecg_get_rri(void);


/** Get the count of detected R-to-R interval values since the ECG pipeline initialization. */
int32_t
nsk_ecg_get_rri_count(void);


/** Get the count of raw ECG samples fed to the ECG pipeline since initialization. */
int32_t
nsk_ecg_get_sample_count(void);


/**
 * Get the Robust Heart Rate (RHR) estimation of the current moment. RHR unit is in beat-per-minute.
 * RHR will be 0 when the algorithm is yet to detect a valid heart beat. The returned RHR can be positive or negative.
 * A negative RHR value indicates that the signal quality was bad and the value is considered as unsettled.
 * In such cases, the library user is advised to either not display such values, or display them in a way that the
 * end user is aware of the low-quality nature of the value.
 * A positive RHR value indicates that the algorithm consider this value confident.
 * We recommend to use this function when the general signal condition is rather poor (e.g. ECG from wrist).
 */
int16_t
nsk_ecg_get_hr_robust(void);


/**
 * Estimate the current Heart Rate, in beat-per-minute. This method differs from @c nsk_get_hr_robust() in that
 * it does not take the signal quality into account, and unlike RHR, it will not return negative values.
 * The computed HR might fluctuate more when the signal quality is not so good. We recommend to use this function when
 * the general signal condition is acceptable (e.g. ECG from finger-tip).
 */
int16_t
nsk_ecg_compute_hr(void);


#ifdef __cplusplus
}
#endif

#endif

#ifndef _WIN32
#pragma GCC visibility pop
#endif

