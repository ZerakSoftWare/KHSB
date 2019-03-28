#include "log.h"

FILE * pFileHandle              = NULL;
int cTrace::ulLogFileHour       = 0;
pthread_mutex_t cTrace::m_mutex;

int gTraceFlag = TRACE_FLOW; 

char trace_flag[TRACE_FLOW + 1][MAX_TRACE_LEN] =
{
    "TRACE_MONITOR",
    "TRACE_ERROR", 
    "TRACE_WARNING", 
    "TRACE_KEY",
    "TRACE_INFO",
    "TRACE_DETAILS", 
    "TRACE_DEBUG",
    "TRACE_FLOW",
};

unsigned int cTrace::Initial()
{
    pthread_mutex_init (&m_mutex, NULL);

    cTrace::set_trace_flag(TRACE_FLOW);
    //printf("gTraceFlag = %s\n", trace_flag[gTraceFlag]);
    
    return true;
}


unsigned int cTrace::Release()
{
    pthread_mutex_destroy(&m_mutex);

    return true;
}

void cTrace::set_trace_flag(int iTraceFlag)
{
    if( iTraceFlag < TRACE_MONITOR || iTraceFlag > TRACE_FLOW )
    {
        printf("set flag[%d] failure!\n", iTraceFlag);
        return ;
    }
    gTraceFlag = iTraceFlag;
    
    return;
}



void  cTrace::trace(TRACE_LEVEL_E eMask, char * pFormat, ...)
{
    /* ��־������� */
    if( eMask > gTraceFlag )
    {
        return;
    }
    
    pthread_mutex_lock(&m_mutex);
    
    va_list listVars;
    char strInfo[MAX_TRACE_LEN]     = {0};
    
    struct timeval tv;
    
    char head [MAX_PATH_LEN]        = {0};
    char wbuff[MAX_TRACE_LEN]       = {0};

    if( strlen(pFormat) < MAX_TRACE_LEN )
    {
        va_start(listVars, pFormat);
        sprintf(strInfo, "[%s]:  ", trace_flag[eMask]);
        vsprintf((char*)(strInfo + strlen(strInfo)), pFormat, listVars);
        va_end(listVars);

        gettimeofday(&tv, NULL);
        localtime(&tv.tv_sec);
        time_t  now ;
        struct tm * ptime;
        time(&now);
        ptime = localtime(&now);
        sprintf(head, "%04i-%02i-%02i %02i:%02i:%02i:%03i ",
                    1900 + ptime->tm_year,
                    ptime->tm_mon+1,
                    ptime->tm_mday,
                    ptime->tm_hour,
                    ptime->tm_min,
                    ptime->tm_sec,
                    (int)(tv.tv_usec /1000));
        strcat((char *)wbuff, head);
        strcat((char *)wbuff, strInfo);
        strcat((char *)wbuff, "\n");

        //printf("ptime->tm_hour = %d, ulLogFileHour = %d, wbuff = %s\n", ptime->tm_hour, ulLogFileHour, wbuff);

        //�Ƚ�ʱ�����ÿСʱһ����־
        if( ptime->tm_hour != ulLogFileHour || pFileHandle == NULL)
        {
            
            //����о��ļ�����رվ��ļ�
            if( pFileHandle != NULL )
            {
                fclose(pFileHandle);
                pFileHandle = NULL;
            }
        
            //�����µ���־�ļ�����������־hour
            pFileHandle = create_log_file(ptime);
            if(pFileHandle == NULL)
            {
                return;
            }
        }

        ulLogFileHour = ptime->tm_hour;
        
        //д��־
        write_log(pFileHandle, wbuff);

    }
    
    pthread_mutex_unlock(&m_mutex);
    
    return;
}

//������־�ļ�
FILE * cTrace::create_log_file(struct tm * ptime)
{
    char ucFileName[MAX_FILENAME_LEN];
    char ucTmp[MAX_FILENAME_LEN];
    
    sprintf(ucTmp, "%04i-%02i-%02i-%02i.log",
                1900 + ptime->tm_year,
                ptime->tm_mon+1,
                ptime->tm_mday,
                ptime->tm_hour);
    
    mkdir("log\/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 

    strcpy((char *)ucFileName, (const char *)"log\/");
    strcat((char *)ucFileName, (const char *)LOG_PRE);
    strcat((char *)ucFileName, (const char *)ucTmp);

    //��дȨ��
    pFileHandle = fopen(ucFileName, "a+");
    if( pFileHandle == NULL )
    {
        printf("fopen(%s, \"a+\") failure!\n", ucFileName);
    }
    printf("fopen success ucFileName = %s!\n", ucFileName);
    
    
    return pFileHandle;
}


//д��־
void cTrace::write_log(FILE * pFileHandle, char * pFormat)
{
    int iRlt = 0;
    
    //pthread_mutex_lock(&m_mutex);

    iRlt = fwrite(pFormat, strlen(pFormat), 1, pFileHandle);
    if ( iRlt != 1 )
    {
        //printf("write_log failure!\n");
    }
    fflush(pFileHandle); 
    
    //pthread_mutex_unlock(&m_mutex);

    if( __SCREEN_PRINTF__ )
    {
        printf("%s", pFormat);
    }
    
    return;
}

