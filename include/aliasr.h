#ifndef __ALIASR_H__
#define __ALIASR_H__

#include "log.h"

#include "nlsClient.h"
#include "nlsEvent.h"
#include "SpeechTranscriberRequest.h"
#include "nlsCommonSdk/Token.h"

#include "queue.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;

using namespace AlibabaNlsCommon;

using AlibabaNls::NlsClient;
using AlibabaNls::NlsEvent;
using AlibabaNls::LogDebug;
using AlibabaNls::LogInfo;
using AlibabaNls::SpeechTranscriberCallback;
using AlibabaNls::SpeechTranscriberRequest;



#define TITLE_ALIASR            "ALIASR"

#define KEY_APP_KEY             "AppKey"
#define KEY_ACCESS_KEY_ID       "AccessKeyID"
#define KEY_ACCESS_KEY_SECRET   "AccessKeySecret"

#define FRAME_SIZE 3200
//#define SAMPLE_RATE 16000

class ALIASR
{
    public:
        
        ALIASR();
        ~ALIASR();
        
        static int Initial();
        static void Release();
        
        static int aliasr_get_config();
        static int aliasr_generate_Token(string akId, string akSecret, string* token);

        int a;
        int ulAsrInfoNumber;        // asr_data_info 
        msg_queue_t * asr_queue;    // 消息队列
        string strTaskId;           // 任务id
        string token;               // 该条任务token
        string uuid;
        
        char ucReason;              // stop_asr_reason: 空号识别结果

        // 队列操作及队列
        int aliasr_push_message(char * pMessage, int len);
        int aliasr_pop_message(char * pMessage, int *len);
        int aliasr_msg_dispath();
        
    private:


       
};

void * ali_asr_thread(void* arg);


#endif
