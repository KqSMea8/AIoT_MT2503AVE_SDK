/*
 * Copyright (c) NeuroSky Inc. All rights reserved.
 */

/**
 * @file nsk_defines.h 
 * @brief This file contains the type definitions of standard data types.
 */

#ifndef __NSK_DEFINES_H__
#define __NSK_DEFINES_H__

#ifdef __cplusplus
extern "C"{
#endif

#if !(defined __stdint_h || defined _MSC_STDINT_H_ || defined _STDINT_H || defined _STDINT_H_ || defined _STDINT)

/** int32_t is defined as a signed 32bit integer. */
typedef signed int int32_t;

/** uint32_t is defined as an unsigned 32bit integer. */
typedef unsigned int uint32_t;

/** int16_t is defined as a signed 16bit integer. */
typedef signed short int16_t;

/** uint32_t is defined as an unsigned 16bit integer. */
typedef unsigned short uint16_t;

/** int8_t is defined as a signed 8bit integer. */
typedef signed char int8_t;

/** uint8_t is defined as an unsigned 8bit integer. */
typedef unsigned char uint8_t;

/** int64_t is defined as a signed 64bit integer. */
/* typedef signed long long int64_t; */

/** uint64_t is defined as an unsigned 64bit integer. */
/* typedef unsigned long long uint64_t; */


#endif

/** floating point number can be configured at compiler time. Default to double. */
#ifdef REAL_IS_FLOAT
typedef float alg_real_t;
#else
typedef double alg_real_t;
#endif

/** TRUE is defined as 1 */
#ifndef TRUE 
#define TRUE 1
#endif

/** FALSE is defined as 0 */
#ifndef FALSE
#define FALSE 0
#endif

#ifdef __cplusplus
}
#endif
#endif


