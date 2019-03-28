#ifndef __ASR_CNTL_H__
#define __ASR_CNTL_H__

#include "khsbdefine.h"

#include "asr_interface.h"

#define TITLE_TCP_SOCKET            "SOCKET"
#define KEY_TCP_ADDRESS             "IP"
#define KEY_TCP_PORT                "Port"

void asr_cntl_initial();
int asr_cntl_process(char * pMessage);

asr_data_info * asr_cntl_get_asr_info(int ulAsrInfoIndex);

int asr_malloc_cntl_info();
int asr_free_cntl_info(int ulAsrInfoIndex);


#endif
