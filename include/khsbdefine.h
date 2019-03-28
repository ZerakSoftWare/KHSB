#ifndef __KHSB_DEFINE_H__
#define __KHSB_DEFINE_H__

#include "aliasr.h"

#define true 1
#define false 0

#define MAX_STRING_LEN 255

#define MAX_ASR_INFO_NUMBER 1000

typedef enum asr_data_info_state_e
{
    asr_state_initial,
    asr_state_allocated,
    
}asr_data_info_state;

typedef struct asr_data_info_t
{
    int      iDataNumber;
    int      iState;
    ALIASR * pAsrObj;

    char    ucTaskId[255];
    
}asr_data_info;

#endif
