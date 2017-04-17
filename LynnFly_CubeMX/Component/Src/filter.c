/*
*  TOBE FIXED: Use filter object instead of repeated code
*
*/


#include "filter.h"
#include "main.h"
#include "FreeRTOS.h"

#include "cmsis_os.h"
#include "string.h"

#define M_PI_F 3.1415926

typedef struct
{
    
    float           _cutoff_freq;
    float           _a1;
    float           _a2;
    float           _b0;
    float           _b1;
    float           _b2;
    float           _delay_element_1;        // buffered sample -1
    float           _delay_element_2;        // buffered sample -2
    
}FILTER_LPF2P_HANDLE_T;




void LPF2pSetCutoffFreq(FILTER_LPF2P_HANDLE_T *lpf2pHandle, float sample_freq, float cutoff_freq)
{
    float fr =0;
    float ohm =0;
    float c =0;

    fr= sample_freq/cutoff_freq;
    ohm=tanf(M_PI_F/fr);
    c=1.0f+2.0f*cosf(M_PI_F/4.0f)*ohm + ohm*ohm;

    lpf2pHandle->_cutoff_freq = cutoff_freq;
    if (lpf2pHandle->_cutoff_freq > 0.0f)
    {
        lpf2pHandle->_b0 = ohm*ohm/c;
        lpf2pHandle->_b1 = 2.0f*lpf2pHandle->_b0;
        lpf2pHandle->_b2 = lpf2pHandle->_b0;
        lpf2pHandle->_a1 = 2.0f*(ohm*ohm-1.0f)/c;
        lpf2pHandle->_a2 = (1.0f-2.0f*cosf(M_PI_F/4.0f)*ohm+ohm*ohm)/c;
    }
}

float Filter_LPF2pApply(FILTER_HANDLE_T filterHandle, float sample)
{
    FILTER_LPF2P_HANDLE_T *lpf2pHandle = (FILTER_LPF2P_HANDLE_T *)filterHandle;

    float delay_element_0 = 0, output=0;
    if (lpf2pHandle->_cutoff_freq <= 0.0f) {
        // no filtering
        return sample;
    }
    else
    {
        delay_element_0 = sample - lpf2pHandle->_delay_element_1 * lpf2pHandle->_a1 - lpf2pHandle->_delay_element_2 * lpf2pHandle->_a2;
        // do the filtering
        if (isnan(delay_element_0) || isinf(delay_element_0)) {
            // don't allow bad values to propogate via the filter
            delay_element_0 = sample;
        }
        output = delay_element_0 * lpf2pHandle->_b0 + lpf2pHandle->_delay_element_1 
                * lpf2pHandle->_b1 + lpf2pHandle->_delay_element_2 * lpf2pHandle->_b2;

        lpf2pHandle->_delay_element_2 = lpf2pHandle->_delay_element_1;
        lpf2pHandle->_delay_element_1 = delay_element_0;

        // return the value.  Should be no need to check limits
        return output;
    }
}

FILTER_HANDLE_T Filter_LPF2pCreate(float sample_freq, float cutoff_freq)
{
    FILTER_LPF2P_HANDLE_T *lpf2pHandle;
    
    lpf2pHandle = (FILTER_LPF2P_HANDLE_T *)osMalloc(sizeof(FILTER_LPF2P_HANDLE_T));
    
    memset(lpf2pHandle, 0, sizeof(FILTER_LPF2P_HANDLE_T));
    
    LPF2pSetCutoffFreq(lpf2pHandle, sample_freq, cutoff_freq);
    return lpf2pHandle;
}


