#include <stdlib.h>

#include "khsbdefine.h"

#include "config.h"
#include "log.h"

#include "khsbesl.h"
#include "aliasr.h"

#include "test.h"

#include "asrcntl.h"

int main()
{
    // ��־ģ���ʼ��
    cTrace::Initial();

    // Configģ���ʼ��
    SetPath((char *)DEFAULT_CONFIG_FILE);

    // asrģ���ʼ��
    ALIASR::Initial();

    // tcpģ���ʼ��
    asr_cntl_initial();

    //asr_cntl_process();

    //testqueue();
    //testaliasr();
    //testmap();

    EslClient::testesl();

    int i = 0;
    while(1)
    {
        //printf("%d\n", i++);
        sleep(1);
    }
    
    return true;
}

