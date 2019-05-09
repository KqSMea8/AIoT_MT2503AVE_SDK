
/*
 * NeuroSky ECG Lite algorithm Library Version string.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-27-2015;
 */

/**
 * @file version.h
 * @brief NeuroSky ECG Lite algorithm Library Version string.
 */

/*
 * Version 2.1.1    08-14-2015;
 *         Changed population baseline parameter in stress.c;
 * Version 2.1.0    08-05-2015;
 *         New Stress algorithm, belongs to extended pipeline.
 * Version 2.0.8    07-20-2015;
 *         New Heart Age that manages its own history data, uses same format as stress.
 * Version 2.0.7    07-10-2015;
 *         Add customized signature.
 * Version 2.0.6    05-28-2015;
 *         Minor parameter tweak in rsa.c;
 * Version 2.0.5    05-13-2015;
 *         Fixed a potential bug in rsa.c.
 * Version 2.0.4    04-30-2015;
 *         Changed overall SQ update logic in sq.c. Modified resampler.c to handle general cases.
 * Version 2.0.3    04-22-2015;
 *         Reworked resampler.c to add upsampling, and adjusted core pipeline update function accordingly.
 * Version 2.0.2    04-15-2015;
 *         Add time domain respiratory rate algorithm and integrate into extended pipeline.
 * Version 2.0.1    04-06-2015;
 *         Backward compatible pipeline added in nsk_ecg_bc.c. Add hard bound to detected RRI.
 * Version 2.0.0    03-11-2015;
 *         Core algorithm: Resampler, Preprocessor, Mechanical noise checker, R-peak detector, Robust Heart Rate.
 *         Extended algoirthm: Mood, HRV;
 */

#ifndef NEL_VERSION_H
#define NEL_VERSION_H

#undef NSK_ECG_LIB_VERSION
#define NSK_ECG_LIB_VERSION "2.1.1"

#endif

