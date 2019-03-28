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
    // ȡ��AppKey / AccessKey ID / Access Key Secret
    aliasr_get_config();

#if 0
    // ����100��ʵ��
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

    // ����Token
    ALIASR::aliasr_generate_Token(AccessKeyID, AccessKeySecret, &AccessToken);

    // ��������
    asr_queue = create_msg_queue();
    
    //�������ղ�����Frame�߳�
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

/* ����AccessKey ID��AccessKey Secret��������һ��token������ȡ����Ч��ʱ��� */
int ALIASR::aliasr_generate_Token(string akId, string akSecret, string* token)
{
    TRALOG(TRACE_DEBUG, "enter ALIASR::aliasr_generate_Token()");

    //string token = "";
    long g_expireTime = -1;

    /* ��ȡ��ǰϵͳʱ������ж�token�Ƿ���� */
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
        // ����ʧ�ܽ��ط�һ��
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
    // ��֧��16λ����
    const int sampleBytes = 16;
    // ��֧�ֵ�ͨ��
    const int soundChannel = 1;

    // ��ǰ�����ʣ�����λ����ÿ��������ݵĴ�С
    int bytes = (sampleRate * sampleBytes * soundChannel) / 8;

    // ��ǰ�����ʣ�����λ����ÿ����������ݵĴ�С
    int bytesMs = bytes / 1000;

    // ���������ݴ�С����ÿ����������ݴ�С���Ի�ȡsleepʱ��
    int sleepMs = (dataSize * compressRate) / bytesMs;

    return sleepMs;
}

void onTranscriptionStarted( NlsEvent* cbEvent, void* cbParam )
{
    cout << "onTranscriptionStarted: "
        << "status code: " << cbEvent->getStausCode()   // ��ȡ��Ϣ��״̬�룬�ɹ�Ϊ0����20000000��ʧ��ʱ��Ӧʧ�ܵĴ�����
        << ", task id: " << cbEvent->getTaskId()        // ��ǰ�����task id�����㶨λ���⣬�������
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
        << "status code: " << cbEvent->getStausCode()   // ��ȡ��Ϣ��״̬�룬�ɹ�Ϊ0����20000000��ʧ��ʱ��Ӧʧ�ܵĴ�����
        << ", task id: " << cbEvent->getTaskId()        // ��ǰ�����task id�����㶨λ���⣬�������
        << ", index: " << cbEvent->getSentenceIndex()   //���ӱ�ţ���1��ʼ����
        << ", time: " << cbEvent->getSentenceTime()     //��ǰ�Ѵ������Ƶʱ������λ�Ǻ���
        << endl;
}

void onSentenceEnd(NlsEvent* cbEvent, void* cbParam) 
{

    cout << "onSentenceEnd: "
         << "status code: " << cbEvent->getStausCode()          // ��ȡ��Ϣ��״̬�룬�ɹ�Ϊ0����20000000��ʧ��ʱ��Ӧʧ�ܵĴ�����
         << ", task id: " << cbEvent->getTaskId()               // ��ǰ�����task id�����㶨λ���⣬�������
         << ", result: " << cbEvent->getResult()                // ��ǰ���ӵ����ʶ����
         << ", index: " << cbEvent->getSentenceIndex()          // ��ǰ���ӵ��������
         << ", time: " << cbEvent->getSentenceTime()            // ��ǰ���ӵ���Ƶʱ��
         << ", begin_time: " << cbEvent->getSentenceBeginTime() // ��Ӧ��SentenceBegin�¼���ʱ��
         << ", confidence: " << cbEvent->getSentenceConfidence()                    // ������Ŷ�,ȡֵ��Χ[0.0,1.0]��ֵԽ���ʾ���Ŷ�Խ��
         << ", stashResult begin_time: " << cbEvent->getStashResultBeginTime()      //��һ�仰��ʼʱ��
         << ", stashResult current_time: " << cbEvent->getStashResultCurrentTime()  //��һ�仰��ǰʱ��
         << ", stashResult Sentence_id: " << cbEvent->getStashResultSentenceId()    //sentence Id
         << ", stashResult Text: " << cbEvent->getStashResultText()                 //��һ�仰ǰ׺
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
        << "status code: " << cbEvent->getStausCode()   // ��ȡ��Ϣ��״̬�룬�ɹ�Ϊ0����20000000��ʧ��ʱ��Ӧʧ�ܵĴ�����
        << ", task id: " << cbEvent->getTaskId()        // ��ǰ�����task id�����㶨λ���⣬�������
        << ", result: " << cbEvent->getResult()         // ��ǰ���ӵ��м�ʶ����
        << ", index: " << cbEvent->getSentenceIndex()   // ��ǰ���ӵ��������
        << ", time: " << cbEvent->getSentenceTime()     // ��ǰ���ӵ���Ƶʱ��
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
        << "status code: " << cbEvent->getStausCode()   // ��ȡ��Ϣ��״̬�룬�ɹ�Ϊ0����20000000��ʧ��ʱ��Ӧʧ�ܵĴ�����
        << ", task id: " << cbEvent->getTaskId()        // ��ǰ�����task id�����㶨λ���⣬�������
        << endl;
}

void onTaskFailed(NlsEvent* cbEvent, void* cbParam) 
{
    cout << "onTaskFailed: "
        << "status code: " << cbEvent->getStausCode()
        << ", task id: " << cbEvent->getTaskId()                // ��ǰ�����task id�����㶨λ���⣬�������
        << ", error message: " << cbEvent->getErrorMessage()
        << endl;
}

void onChannelClosed(NlsEvent* cbEvent, void* cbParam) 
{
    cout << "OnChannelCloseed: All response: " << cbEvent->getAllResponse() << endl;    // getResponse() ����ͨ���ر���Ϣ
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
    
    /* 1: ���������ûص����� */
    callback->setOnTranscriptionStarted(onTranscriptionStarted, (void *)pAsr);
    callback->setOnTranscriptionResultChanged(onTranscriptionResultChanged, (void *)pAsr);  // ����ʶ�����仯�ص�����
    callback->setOnTranscriptionCompleted(onTranscriptionCompleted, (void *)pAsr);          // ��������תд�����ص�����
    callback->setOnSentenceBegin(onSentenceBegin, (void *)pAsr);                            // ����һ�仰��ʼ�ص�����
    callback->setOnSentenceEnd(onSentenceEnd, (void *)pAsr);                                // ����һ�仰�����ص�����
    callback->setOnTaskFailed(onTaskFailed, (void *)pAsr);                                  // �����쳣ʶ��ص�����
    callback->setOnChannelClosed(onChannelClosed, (void *)pAsr);                            // ����ʶ��ͨ���رջص�����
    
    /* 2: ����ʵʱ��Ƶ��ʶ��SpeechTranscriberRequest���� */
    SpeechTranscriberRequest* request = NlsClient::getInstance()->createTranscriberRequest(callback);
    if (request == NULL) 
    {
        cout << "createTranscriberRequest failed." << endl;

        delete callback;
        callback = NULL;

        return NULL;
    }

    // ����AppKey, �������, ����չ�������
    request->setAppKey(AppKey.c_str());
    // ������Ƶ���ݱ����ʽ, Ĭ����pcm
    request->setFormat("pcm"); 
    // ������Ƶ���ݲ�����, ��ѡ������Ŀǰ֧��16000, 8000. Ĭ����16000
    request->setSampleRate(SAMPLE_RATE_8000); 
    // �����Ƿ񷵻��м�ʶ����, ��ѡ����. Ĭ��false
    request->setIntermediateResult(true); 
    // �����Ƿ��ں�������ӱ��, ��ѡ����. Ĭ��false
    request->setPunctuationPrediction(true); 
    // �����Ƿ��ں�����ִ������תд, ��ѡ����. Ĭ��false
    request->setInverseTextNormalization(true); 
    
#if 0
    request->setToken(pa->token.c_str());
#else
    request->setToken(pAsr->token.c_str());
#endif
    
    /* 3: start()Ϊ��������, ����startָ��֮��, ��ȴ��������Ӧ, ��ʱ֮��ŷ��� */
    if (request->start() < 0) 
    {
        cout << "start() failed." << endl;
        NlsClient::getInstance()->releaseTranscriberRequest(request); // start()ʧ�ܣ��ͷ�request����

        delete callback;
        callback = NULL;

        return NULL;
    }

#if 0
    /* ����Ƶ�ļ�, ��ȡ���� */
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
        /* 4: ������Ƶ����. sendAudio����-1��ʾ����ʧ��, ��Ҫֹͣ����. ���ڵ���������:
         * formatΪopu(����ԭʼ��Ƶ���ݱ���ΪPCM, FRAME_SIZE��С����Ϊ640)ʱ, ������Ϊtrue. ������ʽĬ��ʹ��false.*/
        //nlen = request->sendAudio(data, nlen, false);
        nlen = request->sendAudio(pMessage->data, pMessage->data_len, false);
        if (nlen < 0) 
        {
            // ����ʧ��, �˳�ѭ�����ݷ���
            cout << "send data fail." << endl;
            break;
        } 
        else 
        {
            //cout << "send len:" << nlen << " ." << endl;
        }
    
#if 0
        /* �������ݷ��Ϳ��ƣ�
         * ����������ʵʱ��, ����sleep��������, ֱ�ӷ��ͼ���.
         * �������������ļ�, ����ʱ��Ҫ��������, ʹ��λʱ���ڷ��͵����ݴ�С�ӽ���λʱ��ԭʼ�������ݴ洢�Ĵ�С. */
        int sleepMs = getSendAudioSleepTime(nlen, SAMPLE_RATE_16000, 1); // ���� �������ݴ�С�������ʣ�����ѹ���� ����ȡsleepʱ��

        /*
        * 5: �������ݷ�����ʱ����
        */
        usleep(sleepMs * 1000);
#endif
        
    }

    // �ر���Ƶ�ļ�
    //fs.close();
    
    /* 6: ���ݷ��ͽ������ر�ʶ������ͨ��.
    * stop()Ϊ��������, �ڽ��ܵ��������Ӧ, ���߳�ʱ֮��, �Ż᷵��.*/
    request->stop();
    
    /* 7: ʶ�����, �ͷ�request���� */
    NlsClient::getInstance()->releaseTranscriberRequest(request);

    /* 8: �ͷ�callback���� */
    delete callback;
    callback = NULL;
    
    return NULL;
}


