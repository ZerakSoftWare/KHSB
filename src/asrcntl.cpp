#include "asrcntl.h"
#include "aliasr.h"
#include "khsbdefine.h"
#include "tcpserver.h"
#include "util.h"
#include "config.h"
#include "map.h"

static int g_AsrInfoIndex = 0;
int g_AsrInfoNumber = 0;
pthread_mutex_t mutex_asr_info;

asr_data_info m_AsrInfo[MAX_ASR_INFO_NUMBER + 1];

void * asr_cntl_tcp_thread(void * arg)
{
    char    ucAddress[128];
    int     iPort;

    /* tcp server start */
    strcpy((char *)ucAddress, GetIniKeyString((char *)TITLE_TCP_SOCKET, (char *)KEY_TCP_ADDRESS)); 
    //TRALOG(TRACE_INFO, "pAddress = %s", ucAddress);
    
    iPort = GetIniKeyInt((char *)TITLE_TCP_SOCKET, (char *)KEY_TCP_PORT);

    TRALOG(TRACE_INFO, "pAddress = %s, iPort = %d", ucAddress, iPort);

    tcp_server_config(ucAddress, iPort);
    tcp_server_recv();

    return NULL;
}

void asr_cntl_initial()
{
    pthread_mutex_init(&mutex_asr_info, NULL);
    
    /* 启动tcp接收线程 */
    pthread_t pthreadId;
    pthread_create(&pthreadId, NULL, &asr_cntl_tcp_thread, NULL);
    
    return;
}


void asr_cntl_create_queue()
{
    return;
}

void asr_cntl_insert_queue()
{
    
    return;
}

int asr_cntl_process(char * pMessage)
{
    asr_data_info * pAsrInfo = NULL;
        
    //TRALOG(TRACE_FLOW, "enter asr_cntl_process()");

    // 取出消息里的uuid
    fs_asr_message_t * pAsrRequest = (fs_asr_message_t *)pMessage;
    
    char uuid[FS_ASR_STRING_LEN];
    //strcpy((char *)uuid, (const char *)pAsrRequest->uuid);
    //TRALOG(TRACE_INFO, "uuid = %s, len = %d, seq = %d, sample = %d data_len = %d", 
    //    pAsrRequest->uuid, pAsrRequest->msg_len, pAsrRequest->seq, pAsrRequest->rate, pAsrRequest->data_len);

    //printbuf(pMessage, pAsrRequest->msg_len);

    /* 查找uuid映射表，如果没有，则分配 */
    int ulAsrInfoIndex = 0;
    ali_get_map_relation(pAsrRequest->uuid, &ulAsrInfoIndex);
    if( ulAsrInfoIndex == 0 )
    {
        ulAsrInfoIndex = asr_malloc_cntl_info();
        if( ulAsrInfoIndex == 0 )
        {
            TRALOG(TRACE_ERROR, "asr_malloc_cntl_info failure! pAsrRequest->uuid = %s", pAsrRequest->uuid);
            return false;
        }
        TRALOG(TRACE_INFO, "asr_malloc_cntl_info success! pAsrRequest->uuid = %s, ulAsrInfoIndex = %d", 
            pAsrRequest->uuid, ulAsrInfoIndex);
        
        ali_insert_map_relation(pAsrRequest->uuid, ulAsrInfoIndex);

        pAsrInfo = asr_cntl_get_asr_info(ulAsrInfoIndex);

        /* 创建ALIASR对象 */
        ALIASR * pAsrObj = new ALIASR;
        if( pAsrObj == NULL )
        {
            asr_free_cntl_info(ulAsrInfoIndex);
            return false;
        }
        
        pAsrInfo->pAsrObj = pAsrObj;
        pAsrObj->ulAsrInfoNumber = ulAsrInfoIndex;

        //uuid
        pAsrObj->uuid = pAsrRequest->uuid;
        
    }
    else
    {
        pAsrInfo = asr_cntl_get_asr_info(ulAsrInfoIndex);
    }

    switch(pAsrRequest->msg_type)
    {
        case FS_ASR_ASR_DATA:
            //TRALOG(TRACE_INFO, "enter FS_ASR_ASR_DATA");

            /* 设置队列 */
            pAsrInfo->pAsrObj->aliasr_push_message((char *)pAsrRequest, pAsrRequest->msg_len);

            break;
            
        default:
            break;
    }
    
    return true;
}

asr_data_info * asr_cntl_get_asr_info(int ulAsrInfoIndex)
{
    if( ulAsrInfoIndex <= 0 || ulAsrInfoIndex > MAX_ASR_INFO_NUMBER )
    {
        TRALOG(TRACE_ERROR, "ulAsrInfoIndex[%d] error!", ulAsrInfoIndex);
        return false;
    }
    
    return &m_AsrInfo[ulAsrInfoIndex];
}

/* 分配asr_data_info */
int asr_malloc_cntl_info()
{
    TRALOG(TRACE_FLOW, "enter asr_malloc_cntl_info()");

    pthread_mutex_lock(&mutex_asr_info);
    if( g_AsrInfoIndex == MAX_ASR_INFO_NUMBER )
    {
        g_AsrInfoIndex = 0;
    }
    
    while(g_AsrInfoIndex < MAX_ASR_INFO_NUMBER + 1)
    {
        g_AsrInfoIndex++;
        if( m_AsrInfo[g_AsrInfoIndex].iState == asr_state_initial )
        {
            m_AsrInfo[g_AsrInfoIndex].iState = asr_state_allocated;
            m_AsrInfo[g_AsrInfoIndex].iDataNumber = g_AsrInfoIndex;
            
            TRALOG(TRACE_INFO,"malloc g_AsrInfoIndex = %d", g_AsrInfoIndex);
            
            g_AsrInfoNumber++;
            pthread_mutex_unlock(&mutex_asr_info);
            return g_AsrInfoIndex;
        }

        g_AsrInfoIndex++;
    }
    pthread_mutex_unlock(&mutex_asr_info);

    TRALOG(TRACE_INFO, "-n-5--, g_AsrInfoIndex = %d", g_AsrInfoIndex);
    
    return false;
    
}

int asr_free_cntl_info(int ulAsrInfoIndex)
{
    asr_data_info * pAsrInfo = asr_cntl_get_asr_info(ulAsrInfoIndex);

    /* free Aliasr */
    if( pAsrInfo->pAsrObj != NULL )
    {
        delete pAsrInfo->pAsrObj;
        pAsrInfo->pAsrObj = NULL;
    }
    
    pAsrInfo->iState = asr_state_initial;
    
    return true;
}


