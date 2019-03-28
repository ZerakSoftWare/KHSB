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

#ifndef NLS_SDK_SPEECH_TRANSCRIBER_REQUEST_H
#define NLS_SDK_SPEECH_TRANSCRIBER_REQUEST_H

#include "nlsGlobal.h"
#include "iNlsRequest.h"

#if defined(_WIN32)
    #pragma warning( push )
    #pragma warning ( disable : 4251 )
#endif

namespace AlibabaNls {

class SpeechTranscriberParam;

class NLS_SDK_CLIENT_EXPORT SpeechTranscriberCallback {

public:

/**
    * @brief ���캯��
    */
SpeechTranscriberCallback();

/**
    * @brief ��������
    */
~SpeechTranscriberCallback();

/**
    * @brief ���ô���ص�����
    * @note ����������г����쳣����ʱ��sdk�ڲ��߳��ϱ��ûص����û��������¼�����Ϣͷ�м��״̬���״̬��Ϣ����ȷ��ʧ�ܵľ���ԭ��.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnTaskFailed(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ����ʵʱ��Ƶ��ʶ��ʼ�ص�����
    * @note ����˾��������Կ�ʼʶ��ʱ��sdk�ڲ��߳��ϱ��ûص�.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnTranscriptionStarted(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ����һ�仰��ʼ�ص�
    * @note ��⵽һ�仰�Ŀ�ʼʱ, sdk�ڲ��߳��ϱ��ûص�.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnSentenceBegin(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ����ʵʱ��Ƶ��ʶ���м����ص�����
    * @note ����enable_intermediate_result�ֶ�Ϊtrue���Ż����м���.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnTranscriptionResultChanged(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ����һ�仰�����ص�����
    * @note ��⵽��һ�仰�Ľ���ʱ, sdk�ڲ��߳��ϱ��ûص�.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnSentenceEnd(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ���÷���˽�������ص�����
    * @note �ƶ˽���ʵʱ��Ƶ��ʶ�����ʱ, sdk�ڲ��߳��ϱ��ûص�.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣.
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnTranscriptionCompleted(NlsCallbackMethod _event, void* para = NULL);

/**
    * @brief ����ͨ���رջص�����
    * @note �����������ͨ���ر�ʱ��sdk�ڲ��߳��ϱ��ûص�.
    *       �������ڻص������ڲ�����stop(), releaseTranscriberRequest()�������, ������쳣
    * @param _event    �ص�����
    * @param para    �û��������, Ĭ��ΪNULL
    * @return void
    */
void setOnChannelClosed(NlsCallbackMethod _event, void* para = NULL);

NlsCallbackMethod _onTaskFailed;
NlsCallbackMethod _onTranscriptionStarted;
NlsCallbackMethod _onSentenceBegin;
NlsCallbackMethod _onTranscriptionResultChanged;
NlsCallbackMethod _onSentenceEnd;
NlsCallbackMethod _onTranscriptionCompleted;
NlsCallbackMethod _onChannelClosed;
std::map < NlsEvent::EventType, void*> _paramap;

};

class NLS_SDK_CLIENT_EXPORT SpeechTranscriberRequest : public INlsRequest {

public:

/**
    * @brief ���캯��
    * @param cb    �¼��ص��ӿ�
    */
SpeechTranscriberRequest(SpeechTranscriberCallback* cb);

/**
    * @brief ��������
    */
~SpeechTranscriberRequest();

/**
    * @brief ����ʵʱ��Ƶ��ʶ�����URL��ַ
    * @param value ����url�ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setUrl(const char* value);

/**
    * @brief ����appKey
    * @note ����չ���
    * @param value appKey�ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setAppKey(const char* value);

/**
    * @brief ������֤�����е����󶼱���ͨ��SetToken������֤ͨ�����ſ���ʹ��
    * @note token��Ҫ�����ȡ
    * @param value    �����token�ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setToken(const char* value);

/**
    * @brief ������Ƶ���ݱ����ʽ
    * @note  ��ѡ������Ŀǰ֧��pcm, opu. Ĭ����pcm
    * @param value    ��Ƶ���ݱ����ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setFormat(const char* value);

/**
    * @brief ������Ƶ���ݲ�����sample_rate
    * @note ��ѡ������Ŀǰ֧��16000, 8000. Ĭ����1600
    * @param value
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setSampleRate(int value);

/**
    * @brief �����Ƿ񷵻��м�ʶ����
    * @note ��ѡ����. Ĭ��false
    * @param value true �� false
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setIntermediateResult(bool value);

/**
    * @brief �����Ƿ��ں�������ӱ��
    * @note ��ѡ����. Ĭ��false
    * @param value true �� false
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setPunctuationPrediction(bool value);

/**
    * @brief �����Ƿ��ں�����ִ������ת��
    * @note ��ѡ����. Ĭ��false
    * @param value true �� false
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setInverseTextNormalization(bool value);

/**
    * @brief �����Ƿ�ʹ������Ͼ�
    * @note ��ѡ����. Ĭ��false. ���ʹ������Ͼ�, ��vad�Ͼ����ò�����Ч. ����Ϊ�����ϵ.
    * @param value true �� false
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setSemanticSentenceDetection(bool value);

/**
    * @brief ����vad��ֵ
    * @note ��ѡ����. ����ʱ����������ֵ�ᱻ��Ϊ�Ͼ�, �Ϸ�������Χ200��2000(ms), Ĭ��ֵ800ms.
    *         vad�Ͼ�������Ͼ�Ϊ�����ϵ, ����ͬʱʹ��. ���ô�����ǰ, �뽫����Ͼ�setSemanticSentenceDetection����Ϊfalse.
    * @param value vad��ֵ
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setMaxSentenceSilence(int value);

/**
    * @brief ���ö���ģ��
    * @param value    ����ģ��id�ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setCustomizationId(const char * value);

/**
    * @brief ���÷��ȴ�
    * @param value    ���Ʒ��ȴ�id�ַ���
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setVocabularyId(const char * value);

/**
    * @brief ����Socket���ճ�ʱʱ��
    * @note
    * @param value ��ʱʱ��
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setTimeout(int value);

/**
* @brief ��������ı��ı����ʽ
* @note
* @param value �����ʽ UTF-8 or GBK
* @return �ɹ��򷵻�0�����򷵻�-1
*/
int setOutputFormat(const char* value);

/**
    * @brief ��������
    * @note  �ݲ����⿪��
    * @param value ����
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setPayloadParam(const char* value);

/**
    * @brief �����û��Զ������
    * @param value ����
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int setContextParam(const char* value);

/**
    * @brief ����ʵʱ��Ƶ��ʶ��
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int start();

/**
    * @brief ��������ȷ�Ϲرգ�����ֹͣʵʱ��Ƶ��ʶ�����
    * @note �����������ȴ��������Ӧ����10�볬ʱ�Ż᷵��
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int stop();

/**
    * @brief ����������ȷ�Ϲرգ�ֱ�ӹر�ʵʱ��Ƶ��ʶ�����
    * @note ����ʹ��stop()�����������.cancel()���жϷ����������̣����ؽ������Ԥ֪.
    * @return �ɹ��򷵻�0�����򷵻�-1
    */
int cancel();

/**
    * @brief ������������
    * @note request����format����ΪΪopu, encoded������Ϊtrue. ������ʽĬ��Ϊfalse.
    * @param data    ��������
    * @param dataSize    �������ݳ���(����ÿ��100ms��������)
    * @param encoded    �Ƿ�����ѹ��, Ĭ��Ϊfalse����������ѹ��.
    * @return �ɹ��򷵻�ʵ�ʷ��ͳ��ȣ�ʧ�ܷ���-1
    */
int sendAudio(const char* data, int dataSize, bool encoded = false);

/**
    * @brief SDK�ڲ�ʹ��
    */
int getTranscriberResult(std::queue<NlsEvent>* eventQueue);

private:
SpeechTranscriberParam* _transcriberParam;
};

}

#if defined (_WIN32)
    #pragma warning( pop )
#endif

#endif //NLS_SDK_SPEECH_TRANSCRIBER_REQUEST_H
