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
    // 日志模块初始化
    cTrace::Initial();

    // Config模块初始化
    SetPath((char *)DEFAULT_CONFIG_FILE);

    // asr模块初始化
    ALIASR::Initial();

    // tcp模块初始化
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

