/*
 * Mood.
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Siyi Deng; 02-27-2015;
 */

/**
 * @file mood.h
 * @brief Mood is the opposite of Relaxation level, and the value is dampened to bounce less.
 */

#ifndef NEL_MOOD_H
#define NEL_MOOD_H

#include "nsk_defines.h"


/** The state data struct */
typedef struct nm_s {
    uint32_t pl;
    uint32_t ph;
    uint32_t nb;
    int16_t rm;
} nm_t;


/** Initialize the state data */
void
nel_mood_init(nm_t* ss);


/**
 * Compute the mood value using @c spectr.c.
 * @param[in, t] ss the current mood state data struct.
 * @param[in] rri array of RR intervals in sample unit.
 * @param[in] n the length of the rri array.
 * @return mood value from 1-100.
 */
int16_t
nel_mood_evaluate(nm_t* ss, const int16_t* rri, const int32_t n);


#endif

