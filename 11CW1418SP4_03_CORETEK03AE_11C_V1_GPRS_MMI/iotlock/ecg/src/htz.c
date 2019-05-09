/*
 * ECG Heart Training Zone
 * Copyright (c) NeuroSky Inc. All rights reserved.
 * Kelvin Soo; 2017-07-08;
 */

#include <string.h>

#include "htz.h"

/* See header file for interface details. */
int8_t 
nel_get_heart_training_zone( 
    const int8_t gender,
    const int8_t age, 
    const int16_t weight,
    const int16_t hr
) {
    int16_t maxHeartRate;
    
    if( (gender < 0) || (gender > 1) ) return -1;
    if( (age < 16) || (age > 90) ) return -1;
    if( (weight < 1) || (weight > 300) ) return -1;
    if( (hr < 30) || (hr > 220) ) return -1;
    
    maxHeartRate= 
        210
        - (age/2) 
        - (weight * 0.11)
        + ((gender == 0) ? 4 : 0);

    if( hr > maxHeartRate * 0.90 ) return 5;
    else if( hr > maxHeartRate * 0.80 ) return 4;
    else if( hr > maxHeartRate * 0.70 ) return 3;
    else if( hr > maxHeartRate * 0.60 ) return 2;
    else if( hr > maxHeartRate * 0.50 ) return 1;
    else return 0;
    
} /* end nel_get_heart_training_zone() */
