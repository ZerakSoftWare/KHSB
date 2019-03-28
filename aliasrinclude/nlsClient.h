/*
 * Copyright 2015 Alibaba Group Holding Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NLS_SDK_CLIENT_H
#define NLS_SDK_CLIENT_H

#include "nlsGlobal.h"

#if defined(_WIN32)
    #include "pthread.h"
#else
    #include <pthread.h>
#endif

#include <string>

namespace AlibabaNls {

class SpeechRecognizerCallback;
class SpeechRecognizerRequest;
class SpeechRecognizerSyncRequest;
class SpeechTranscriberCallback;
class SpeechTranscriberRequest;
class SpeechTranscriberSyncRequest;
class SpeechSynthesizerCallback;
class SpeechSynthesizerRequest;

enum LogLevel {
    LogError = 1,
    LogWarning,
    LogInfo,
    LogDebug
};

class NLS_SDK_CLIENT_EXPORT NlsClient {

public:

/**
    * @brief ������־�ļ���洢·��
    * @param logOutputFile    ��־�ļ�
    * @param logLevel    ��־����Ĭ��1��LogError : 1, LogWarning : 2, LogInfo : 3, LogDebug : 4��
    * @param logFileSize ��־�ļ��Ĵ�С����MBΪ��λ��Ĭ��Ϊ10MB��
    *                    �����־�ļ����ݵĴ�С�������ֵ��SDK���Զ����ݵ�ǰ����־�ļ������ɱ���5���ļ����������ѭ�����������ļ�
    * @return �ɹ��򷵻�0��ʧ�ܷ���-1
    */
int setLogConfig(const char* logOutputFile, const LogLevel logLevel, unsigned int logFileSize = 10);

/**
    * @brief ����һ�仰ʶ�����
    * @param onResultReceivedEvent    �¼��ص��ӿ�
    * @return �ɹ�����speechRecognizerRequest���󣬷��򷵻�NULL
    */
SpeechRecognizerRequest* createRecognizerRequest(SpeechRecognizerCallback* onResultReceivedEvent);

/**
    * @brief ����һ�仰ʶ�����
    * @param request  createRecognizerRequest��������request����
    * @return
    */
void releaseRecognizerRequest(SpeechRecognizerRequest* request);

/**
    * @brief ����һ�仰ͬ��ʶ�����
    * @return �ɹ�����SpeechRecognizerSyncRequest���󣬷��򷵻�NULL
    */
SpeechRecognizerSyncRequest* createRecognizerSyncRequest();

/**
    * @brief ����һ�仰ͬ��ʶ�����
    * @param request  createRecognizerSyncRequest��������request����
    * @return
    */
void releaseRecognizerSyncRequest(SpeechRecognizerSyncRequest* request);

/**
    * @brief ����ʵʱ��Ƶ��ʶ�����
    * @param onResultReceivedEvent    �¼��ص��ӿ�
    * @return �ɹ�����SpeechTranscriberRequest���󣬷��򷵻�NULL
    */
SpeechTranscriberRequest* createTranscriberRequest(SpeechTranscriberCallback* onResultReceivedEvent);

/**
    * @brief ����ʵʱ��Ƶ��ʶ�����
    * @param request  createTranscriberRequest��������request����
    * @return
    */
void releaseTranscriberRequest(SpeechTranscriberRequest* request);

/**
    * @brief ����ʵʱ��Ƶ��ͬ��ʶ�����
    * @return �ɹ�����SpeechTranscriberSyncRequest���󣬷��򷵻�NULL
    */
SpeechTranscriberSyncRequest* createTranscriberSyncRequest();

/**
    * @brief ����ʵʱ��Ƶ��ͬ��ʶ�����
    * @param request  createTranscriberSyncRequest��������request����
    * @return
    */
void releaseTranscriberSyncRequest(SpeechTranscriberSyncRequest* request);

/**
    * @brief ���������ϳɶ���
    * @param onResultReceivedEvent    �¼��ص��ӿ�
    * @return �ɹ���SpeechSynthesizerRequest���󣬷��򷵻�NULL
    */
SpeechSynthesizerRequest* createSynthesizerRequest(SpeechSynthesizerCallback* onResultReceivedEvent);

/**
    * @brief ���������ϳɶ���
    * @param request  createSynthesizerRequest��������request����
    * @return
    */
void releaseSynthesizerRequest(SpeechSynthesizerRequest* request);

/**
    * @brief NlsClient����ʵ��
    * @param sslInitial    �Ƿ��ʼ��openssl �̰߳�ȫ��Ĭ��Ϊtrue
    * @return NlsClient����
    */
static NlsClient* getInstance(bool sslInitial = true);

/**
    * @brief ����NlsClient����ʵ��
    * @note �����˳�ʱ����, ����NlsClient.
    * @return
    */
static void releaseInstance();

#if defined(__linux__)
/**
    * @brief ��ȡip
    * @param vipServerDomain ��ַ�������б�. exg: "123.123.123.123:80,124.124.124.124:81"
    * @param targetDomain    ������������
    * @param nlsServerUrl    ��������Ŀ��url
    * @return �ɹ�����0, ʧ�ܷ���-1
    */
static int vipServerListGetUrl(const std::string& vipServerDomainList, const std::string& targetDomain, std::string& url);

#endif

private:

NlsClient();
~NlsClient();

static pthread_mutex_t _mtx;
static bool _isInitializeSSL;
static NlsClient* _instance;

#if defined(__linux__)
static bool _isInitalizeVsClient;
static pthread_mutex_t _mtxVsClient;

static int vipServerGetIp(const std::string& vipServerDomain, const int vipServerPort, const std::string& targetDomain, std::string& nlsServerUrl);
#endif

};

}

#endif //NLS_SDK_CLIENT_H
