#ifndef __TRACE_H__
#define __TRACE_H__

#include <stdio.h>
#include <iostream>
#include "string.h"
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/stat.h> 


using namespace std;

//是否打印printf
#define __SCREEN_PRINTF__   1


#define LOG_PRE "log_khsb_"

//日志最大长度
#define MAX_TRACE_LEN       3072

//最大路径名长度
#define MAX_PATH_LEN        255

//日志文件最大文件名长度
#define MAX_FILENAME_LEN    255

//mutex初始化
//#define PTHREAD_MUTEX_INITIALIZER { { 0, 0, 0, 0, 0, 0, { 0, 0 } } } 
typedef enum TRACE_LEVEL
{
    TRACE_MONITOR ,
    TRACE_ERROR   ,
    TRACE_WARNING ,
    TRACE_KEY     ,
    TRACE_INFO    ,
    TRACE_DETAILS ,
    TRACE_DEBUG   ,
    TRACE_FLOW    ,
    
}TRACE_LEVEL_E;

class cTrace
{
    public :
        
        static unsigned int Initial();
        static unsigned int Release();
        
        //创建日志文件
        static FILE * create_log_file(struct tm * ptime);

        //写日志
        static void write_log(FILE * pFileHandle, char * pFormat);
        
        //日志接口
        static void trace(TRACE_LEVEL_E eMask, char * pFormat, ...);
        static void set_trace_flag(int iTraceFlag);
        
    private :
        //日志文件handle
        //static FILE * pFileHandle;

        //日志文件小时，每小时一个日志文件
        static int ulLogFileHour;

        static pthread_mutex_t m_mutex;
        
};

#define TRALOG(eMask, pFormat,...)\
     cTrace::trace(eMask, (char *)"Func:%s, Line:%u, "pFormat, __func__, __LINE__, ##__VA_ARGS__);


#endif

