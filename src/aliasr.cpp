#include <stdlib.h>
#include <string.h>
#include "aliasr.h"

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <map>

#include <pthread.h>
#include <unistd.h>

#include "asr_interface.h"
#include "config.h"
#include "khsbdefine.h"
#include "util.h"
#include "tcpserver.h"
#include "map.h"
#include "asrcntl.h"

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

/*  AppKey           AccessKeyID      AccessKeySecret
 *  jaKnIyQ2f69ZXxqI LTAIu7a3tu8FVWLh yS4n8Tt2mRNdPHk4N4FsK3uL0Qq0hT
 */
string AppKey = "";
string AccessKeyID = "";
string AccessKeySecret = "";
string AccessToken = "";

map<string, int> map_taskid;

struct Aliasr_param
{
    string token;
    
};

int ALIASR::Initial()
{
    // 取得AppKey / AccessKey ID / Access Key Secret
    aliasr_get_config();

#if 0
    // 生成100个实例
    for( int i = 0; i < 3; i++ )
    {
        ALIASR aliasr;
    }
#endif
    
    return true;
}

void ALIASR::Release()
{
    return;
}


ALIASR::ALIASR()
{
    TRALOG(TRACE_DEBUG, "enter ALIASR::ALIASR()");

    // 计算Token
    ALIASR::aliasr_generate_Token(AccessKeyID, AccessKeySecret, &AccessToken);

    // 创建队列
    asr_queue = create_msg_queue();
    
    //启动接收并发送Frame线程
    pthread_t pthreadId;

    ALIASR * pAsr = this;
    pAsr->a = 2000;
    pAsr->token = AccessToken;

    #if 0
    Aliasr_param pa;
    pa.token = AccessToken;
    #endif
    
    pthread_create(&pthreadId, NULL, &ali_asr_thread, (void *)pAsr);
    
    //pthread_join(pthreadId, NULL);
    
    return;
}

ALIASR::~ALIASR()
{
    release_msg_queue(asr_queue);
    
    return;
}

int ALIASR::aliasr_get_config()
{
#if 1
    AppKey          = GetIniKeyString((char *)TITLE_ALIASR, (char *)KEY_APP_KEY); 
    AccessKeyID     = GetIniKeyString((char *)TITLE_ALIASR, (char *)KEY_ACCESS_KEY_ID); 
    AccessKeySecret = GetIniKeyString((char *)TITLE_ALIASR, (char *)KEY_ACCESS_KEY_SECRET);
#else
    AppKey          = "jaKnIyQ2f69ZXxqI"; 
    AccessKeyID     = "LTAIu7a3tu8FVWLh"; 
    AccessKeySecret = "yS4n8Tt2mRNdPHk4N4FsK3uL0Qq0hT";
#endif

    TRALOG(TRACE_INFO, "AppKey = %s", AppKey.c_str());
    TRALOG(TRACE_INFO, "AccessKeyID = %s", AccessKeyID.c_str());
    TRALOG(TRACE_INFO, "AccessKeySecret = %s", AccessKeySecret.c_str());

    return true;
}

int ALIASR::aliasr_push_message(char * pMessage, int len)
{
    if( asr_queue == NULL )
    {
        TRALOG(TRACE_INFO, "asr_queue == NULL");
        return false;
    }
    //printbuf(pMessage, len);
    push_message(asr_queue, pMessage, len);
    
    return true;
}

int ALIASR::aliasr_pop_message(char * pMessage, int *len)
{
    #if 0
    int ilen = 0;
    sem_wait(&(asr_queue->sem_msg_queue));
    pop_message(asr_queue, pMessage, &ilen);
    *len = ilen;
    #endif
    return true;
}

int ALIASR::aliasr_msg_dispath()
{
    TRALOG(TRACE_DEBUG, "enter ALIASR::aliasr_msg_dispath()");
    
    return true;
}

/* 根据AccessKey ID和AccessKey Secret重新生成一个token，并获取其有效期时间戳 */
int ALIASR::aliasr_generate_Token(string akId, string akSecret, string* token)
{
    TRALOG(TRACE_DEBUG, "enter ALIASR::aliasr_generate_Token()");

    //string token = "";
    long g_expireTime = -1;

    /* 获取当前系统时间戳，判断token是否过期 */
    std::time_t curTime = std::time(0);
    if (g_expireTime - curTime < 10) 
    {
        cout << "the token will be expired, please generate new token by AccessKey-ID and AccessKey-Secret." << endl;
        NlsToken nlsTokenRequest;
        printf("akId = %s\n", akId.c_str());
        printf("akSecret = %s\n", akSecret.c_str());
        nlsTokenRequest.setAccessKeyId(akId);
        nlsTokenRequest.setKeySecret(akSecret);
        
        printf("--1--, token = %s, expireTime = %ld\n", token->c_str(), g_expireTime);
        
        if (-1 == nlsTokenRequest.applyNlsToken()) 
        {
            TRALOG(TRACE_ERROR, "applyNlsToken failure: %s", nlsTokenRequest.getErrorMsg());
            return -1;
        }
        
        *token = nlsTokenRequest.getToken();
        g_expireTime = nlsTokenRequest.getExpireTime();
        printf("--2--, token = %s, expireTime = %ld\n", token->c_str(), g_expireTime);
    }
    
    return 0;
}

int aliasr_compare_string(char * pSrc, char * pDest) 
{
    //printf("enter aliasr_compare_string()\n ");
    string a = pSrc;
    string b = pDest;

    int idx = a.find(b);
    if (idx == string::npos)
    {
        //printf("--1--, a:%s b:%s\n", a.c_str(), b.c_str());    
        return false;
    }
    //printf("--2--, a:%s b:%s\n", a.c_str(), b.c_str());    
    
    return true;
}

int kh_check(ALIASR * pAsrObj,  char * pSrc)
{
    int rlt = false;
    
    rlt = aliasr_compare_string(pSrc, (char *)GetIniKeyString("CHARACTER", "C1"));
    if( rlt == true )
    {
        pAsrObj->ucReason = REASON_NOT_AVAILABLE;
        return true;
    }
    
    rlt = aliasr_compare_string(pSrc, (char *)GetIniKeyString("CHARACTER", "C2"));
    if( rlt == true )
    {
        pAsrObj->ucReason = REASON_POWER_OFF;
        return true;
    }
    
    rlt = aliasr_compare_string(pSrc, (char *)GetIniKeyString("CHARACTER", "C3"));
    if( rlt == true )
    {
        pAsrObj->ucReason = REASON_BUSY;
        return true;
    }
    
    rlt = aliasr_compare_string(pSrc, (char *)GetIniKeyString("CHARACTER", "C4"));
    if( rlt == true )
    {
        pAsrObj->ucReason = REASON_NOT_IN_SERVICE;
        return true;
    }
    
    return false;
}


void aliasr_send_stop_asr_request(ALIASR * pAsrObj)
{
    TRALOG(TRACE_FLOW, "enter aliasr_send_stop_asr_request()");
    printf("enter aliasr_send_stop_asr_request()\n");

    asr_fs_message stMessage;

    stMessage.header.msg_type = ASR_FS_STOP_ASR;
    stMessage.header.msg_len = sizeof(asr_fs_message);

    strcpy((char *)stMessage.stStopAsrRequest.uuid, (const char *)pAsrObj->uuid.c_str());
    stMessage.stStopAsrRequest.reason = pAsrObj->ucReason;

    int count = tcp_server_send((char *)&stMessage, sizeof(asr_fs_message));
    if( count <= 0)
    {
        //printf("--1--, count = %d\n", count);
        // 发送失败仅重发一次
        tcp_server_send((char *)&stMessage, sizeof(asr_fs_message));
    }
    //printf("--2--, count = %d\n", count);
    
#if 0
    typedef struct asr_fs_header_t
    {
        int msg_type;
        int msg_len;
    }asr_fs_header;
    
    
    typedef struct asr_fs_stop_asr_request_t
    {
        char uuid[FS_ASR_STRING_LEN];
        
        // stop_asr_reason
        char reason;    
        
    }asr_fs_stop_asr_request;
    
    typedef struct asr_fs_message_t
    {
        asr_fs_header header;
        
        asr_fs_stop_asr_request stStopAsrRequest;
        
    }asr_fs_message;
#endif
    
    return;
}


unsigned int getSendAudioSleepTime(const int dataSize,
                                   const int sampleRate,
                                   const int compressRate) 
{
    // 仅支持16位采样
    const int sampleBytes = 16;
    // 仅支持单通道
    const int soundChannel = 1;

    // 当前采样率，采样位数下每秒采样数据的大小
    int bytes = (sampleRate * sampleBytes * soundChannel) / 8;

    // 当前采样率，采样位数下每毫秒采样数据的大小
    int bytesMs = bytes / 1000;

    // 待发送数据大小除以每毫秒采样数据大小，以获取sleep时间
    int sleepMs = (dataSize * compressRate) / bytesMs;

    return sleepMs;
}

void onTranscriptionStarted( NlsEvent* cbEvent, void* cbParam )
{
    cout << "onTranscriptionStarted: "
        << "status code: " << cbEvent->getStausCode()   // 获取消息的状态码，成功为0或者20000000，失败时对应失败的错误码
        << ", task id: " << cbEvent->getTaskId()        // 当前任务的task id，方便定位问题，建议输出
        << endl;
    
    
    ALIASR * pAsrObj = (ALIASR *)cbParam;
    if( pAsrObj == NULL )
    {
        TRALOG(TRACE_ERROR, "pAsr == NULL");
        return;
    }

    pAsrObj->strTaskId = cbEvent->getTaskId();
    
    _insert_map_relation(map_taskid, (char *)pAsrObj->strTaskId.c_str(), pAsrObj->ulAsrInfoNumber);
    TRALOG(TRACE_INFO, "pAsrObj->strTaskId = %s, pAsrObj->ulAsrInfoNumber = %d", pAsrObj->strTaskId.c_str(), pAsrObj->ulAsrInfoNumber);
    
    return;
}

void onSentenceBegin(NlsEvent* cbEvent, void* cbParam) 
{
    cout << "onSentenceBegin: "
        << "status code: " << cbEvent->getStausCode()   // 获取消息的状态码，成功为0或者20000000，失败时对应失败的错误码
        << ", task id: " << cbEvent->getTaskId()        // 当前任务的task id，方便定位问题，建议输出
        << ", index: " << cbEvent->getSentenceIndex()   //句子编号，从1开始递增
        << ", time: " << cbEvent->getSentenceTime()     //当前已处理的音频时长，单位是毫秒
        << endl;
}

void onSentenceEnd(NlsEvent* cbEvent, void* cbParam) 
{

    cout << "onSentenceEnd: "
         << "status code: " << cbEvent->getStausCode()          // 获取消息的状态码，成功为0或者20000000，失败时对应失败的错误码
         << ", task id: " << cbEvent->getTaskId()               // 当前任务的task id，方便定位问题，建议输出
         << ", result: " << cbEvent->getResult()                // 当前句子的完成识别结果
         << ", index: " << cbEvent->getSentenceIndex()          // 当前句子的索引编号
         << ", time: " << cbEvent->getSentenceTime()            // 当前句子的音频时长
         << ", begin_time: " << cbEvent->getSentenceBeginTime() // 对应的SentenceBegin事件的时间
         << ", confidence: " << cbEvent->getSentenceConfidence()                    // 结果置信度,取值范围[0.0,1.0]，值越大表示置信度越高
         << ", stashResult begin_time: " << cbEvent->getStashResultBeginTime()      //下一句话开始时间
         << ", stashResult current_time: " << cbEvent->getStashResultCurrentTime()  //下一句话当前时间
         << ", stashResult Sentence_id: " << cbEvent->getStashResultSentenceId()    //sentence Id
         << ", stashResult Text: " << cbEvent->getStashResultText()                 //下一句话前缀
         << endl;
    
    ALIASR * pAsrObj = (ALIASR *)cbParam;
    if( pAsrObj == NULL )
    {
        TRALOG(TRACE_ERROR, "pAsr == NULL");
        return;
    }

    int rlt = kh_check(pAsrObj, (char *)cbEvent->getResult());
    if ( rlt == true )    
    {
        aliasr_send_stop_asr_request(pAsrObj);
        asr_free_cntl_info(pAsrObj->ulAsrInfoNumber);
    }
    

#if 0
    char ucBString[255];
    memset((void *)ucBString, 0, 255);
    strcpy((char *)ucBString, (const char *)GetIniKeyString("CHARACTER", "C1"));
    
    int len = strlen(ucBString);
    int rlt_len = strlen(cbEvent->getResult());

    printf("len = %d, rlt_len = %d\n", len, rlt_len);

    if( rlt_len >= len )
    {
        int rlt = aliasr_compare_string((char *)cbEvent->getResult(), (char *)ucBString);
        if ( rlt == true )    
        {
            aliasr_send_stop_asr_request();
        }
    }
#endif
}

void onTranscriptionResultChanged(NlsEvent* cbEvent, void* cbParam)
{
    cout << "onTranscriptionResultChanged: "
        << "status code: " << cbEvent->getStausCode()   // 获取消息的状态码，成功为0或者20000000，失败时对应失败的错误码
        << ", task id: " << cbEvent->getTaskId()        // 当前任务的task id，方便定位问题，建议输出
        << ", result: " << cbEvent->getResult()         // 当前句子的中间识别结果
        << ", index: " << cbEvent->getSentenceIndex()   // 当前句子的索引编号
        << ", time: " << cbEvent->getSentenceTime()     // 当前句子的音频时长
        << endl;

    
    ALIASR * pAsrObj = (ALIASR *)cbParam;
    if( pAsrObj == NULL )
    {
        TRALOG(TRACE_ERROR, "pAsr == NULL");
        return;
    }

    int rlt = kh_check(pAsrObj, (char *)cbEvent->getResult());
    if ( rlt == true )    
    {
        aliasr_send_stop_asr_request(pAsrObj);
        asr_free_cntl_info(pAsrObj->ulAsrInfoNumber);
    }
    
}

void onTranscriptionCompleted(NlsEvent* cbEvent, void* cbParam) 
{

    cout << "onTranscriptionCompleted: "
        << "status code: " << cbEvent->getStausCode()   // 获取消息的状态码，成功为0或者20000000，失败时对应失败的错误码
        << ", task id: " << cbEvent->getTaskId()        // 当前任务的task id，方便定位问题，建议输出
        << endl;
}

void onTaskFailed(NlsEvent* cbEvent, void* cbParam) 
{
    cout << "onTaskFailed: "
        << "status code: " << cbEvent->getStausCode()
        << ", task id: " << cbEvent->getTaskId()                // 当前任务的task id，方便定位问题，建议输出
        << ", error message: " << cbEvent->getErrorMessage()
        << endl;
}

void onChannelClosed(NlsEvent* cbEvent, void* cbParam) 
{
    cout << "OnChannelCloseed: All response: " << cbEvent->getAllResponse() << endl;    // getResponse() 可以通道关闭信息
}

void * ali_asr_thread(void* arg) 
{
    TRALOG(TRACE_FLOW, "enter ali_asr_thread()");

    SpeechTranscriberCallback * callback = NULL;

    ALIASR * pAsr = (ALIASR *)arg;
    printf("pAsr.a = %d\n", pAsr->a);
    
#if 0
    Aliasr_param* pa = (Aliasr_param*)arg;
    //struct Aliasr_param * pa = (struct Aliasr_param *)arg;
    if( pa == NULL )
    {
        TRALOG(TRACE_ERROR, "pa = NULL");
        return NULL;
    }
    printf("pa.token = %s\n", pa->token.c_str());
#endif
    callback = new SpeechTranscriberCallback();
    
    /* 1: 创建并设置回调函数 */
    callback->setOnTranscriptionStarted(onTranscriptionStarted, (void *)pAsr);
    callback->setOnTranscriptionResultChanged(onTranscriptionResultChanged, (void *)pAsr);  // 设置识别结果变化回调函数
    callback->setOnTranscriptionCompleted(onTranscriptionCompleted, (void *)pAsr);          // 设置语音转写结束回调函数
    callback->setOnSentenceBegin(onSentenceBegin, (void *)pAsr);                            // 设置一句话开始回调函数
    callback->setOnSentenceEnd(onSentenceEnd, (void *)pAsr);                                // 设置一句话结束回调函数
    callback->setOnTaskFailed(onTaskFailed, (void *)pAsr);                                  // 设置异常识别回调函数
    callback->setOnChannelClosed(onChannelClosed, (void *)pAsr);                            // 设置识别通道关闭回调函数
    
    /* 2: 创建实时音频流识别SpeechTranscriberRequest对象 */
    SpeechTranscriberRequest* request = NlsClient::getInstance()->createTranscriberRequest(callback);
    if (request == NULL) 
    {
        cout << "createTranscriberRequest failed." << endl;

        delete callback;
        callback = NULL;

        return NULL;
    }

    // 设置AppKey, 必填参数, 请参照官网申请
    request->setAppKey(AppKey.c_str());
    // 设置音频数据编码格式, 默认是pcm
    request->setFormat("pcm"); 
    // 设置音频数据采样率, 可选参数，目前支持16000, 8000. 默认是16000
    request->setSampleRate(SAMPLE_RATE_8000); 
    // 设置是否返回中间识别结果, 可选参数. 默认false
    request->setIntermediateResult(true); 
    // 设置是否在后处理中添加标点, 可选参数. 默认false
    request->setPunctuationPrediction(true); 
    // 设置是否在后处理中执行数字转写, 可选参数. 默认false
    request->setInverseTextNormalization(true); 
    
#if 0
    request->setToken(pa->token.c_str());
#else
    request->setToken(pAsr->token.c_str());
#endif
    
    /* 3: start()为阻塞操作, 发送start指令之后, 会等待服务端响应, 或超时之后才返回 */
    if (request->start() < 0) 
    {
        cout << "start() failed." << endl;
        NlsClient::getInstance()->releaseTranscriberRequest(request); // start()失败，释放request对象

        delete callback;
        callback = NULL;

        return NULL;
    }

#if 0
    /* 打开音频文件, 获取数据 */
    ifstream fs;
    fs.open("test1.wav", ios::binary | ios::in);
    if (!fs) 
    {
        cout << "test1.wav" << " isn't exist.." << endl;
        return NULL;
    }
#endif

    while(1)
    {
        char data[FRAME_SIZE] = {0};

#if 1
        int nlen = 0;
        sem_wait(&(pAsr->asr_queue->sem_msg_queue));
        pop_message(pAsr->asr_queue, data, &nlen);

        fs_asr_message * pMessage = (fs_asr_message *)data;
#else
        fs.read(data, sizeof(char) * FRAME_SIZE);
        int nlen = (int)fs.gcount();
#endif
        /* 4: 发送音频数据. sendAudio返回-1表示发送失败, 需要停止发送. 对于第三个参数:
         * format为opu(发送原始音频数据必须为PCM, FRAME_SIZE大小必须为640)时, 需设置为true. 其它格式默认使用false.*/
        //nlen = request->sendAudio(data, nlen, false);
        nlen = request->sendAudio(pMessage->data, pMessage->data_len, false);
        if (nlen < 0) 
        {
            // 发送失败, 退出循环数据发送
            cout << "send data fail." << endl;
            break;
        } 
        else 
        {
            //cout << "send len:" << nlen << " ." << endl;
        }
    
#if 0
        /* 语音数据发送控制：
         * 语音数据是实时的, 不用sleep控制速率, 直接发送即可.
         * 语音数据来自文件, 发送时需要控制速率, 使单位时间内发送的数据大小接近单位时间原始语音数据存储的大小. */
        int sleepMs = getSendAudioSleepTime(nlen, SAMPLE_RATE_16000, 1); // 根据 发送数据大小，采样率，数据压缩比 来获取sleep时间

        /*
        * 5: 语音数据发送延时控制
        */
        usleep(sleepMs * 1000);
#endif
        
    }

    // 关闭音频文件
    //fs.close();
    
    /* 6: 数据发送结束，关闭识别连接通道.
    * stop()为阻塞操作, 在接受到服务端响应, 或者超时之后, 才会返回.*/
    request->stop();
    
    /* 7: 识别结束, 释放request对象 */
    NlsClient::getInstance()->releaseTranscriberRequest(request);

    /* 8: 释放callback对象 */
    delete callback;
    callback = NULL;
    
    return NULL;
}


