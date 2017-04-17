#ifndef _FILTER_H
#define _FILTER_H

#include <math.h>


typedef void * FILTER_HANDLE_T;

FILTER_HANDLE_T Filter_LPF2pCreate(float sample_freq, float cutoff_freq);
float Filter_LPF2pApply(FILTER_HANDLE_T filterHandle, float sample);
#endif

