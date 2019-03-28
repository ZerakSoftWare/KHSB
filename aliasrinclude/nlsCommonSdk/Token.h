/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ALIBABANLS_COMMON_TOKEN_H_
#define ALIBABANLS_COMMON_TOKEN_H_

#include "Global.h"
#include <string>

namespace AlibabaNlsCommon {

    class ALIBABANLS_COMMON_EXPORT NlsToken {

    public:

        virtual ~NlsToken();
        NlsToken();

         /**
            * @brief �����ȡtokenId.
            * @note ����֮ǰ, ���������������.
            * @return �ɹ��򷵻�0; ʧ�ܷ���-1.
            */
         int applyNlsToken();

         /**
             * @brief ��ȡ������Ϣ.
             * @return �ɹ��򷵻ش�����Ϣ; ʧ�ܷ���NULL.
             */
         const char* getErrorMsg();

         /**
             * @brief ��ȡtokenId.
             * @return �ɹ��򷵻�token�ַ���; ʧ�ܷ���NULL.
             */
         const char* getToken();

         /**
             * @brief token��Ч��ʱ���(��).
             * @return �ɹ��򷵻���Ч��ʱ���, ʧ�ܷ���0.
             */
         unsigned int getExpireTime();

        /**
            * @brief ���ð������˺ŵ�KeySecret
            * @param KeySecret    Secret�ַ���
            * @return void
            */
        void setKeySecret(const std::string & KeySecret);

        /**
            * @brief ���ð������˺ŵ�KeyId
            * @param KeyId    Id�ַ���
            * @return void
            */
        void setAccessKeyId(const std::string & accessKeyId);

        /**
            * @brief ��������
            * @param domain    �����ַ���
            * @return void
            */
        void setDomain(const std::string & domain);

        /**
            * @brief ����API�汾
            * @param serverVersion    API�汾�ַ���
            * @return void
            */
        void setServerVersion(const std::string & serverVersion);

        /**
            * @brief ���÷���·��
            * @param serverResourcePath    ����·���ַ���
            * @return void
            */
        void setServerResourcePath(const std::string & serverResourcePath);

    private:
        std::string accessKeySecret_;
        std::string accessKeyId_;

        std::string domain_;
        std::string serverVersion_;
        std::string serverResourcePath_;

        std::string errorMsg_;
        std::string tokenId_;
        unsigned int expireTime_;

        int paramCheck();
    };

}

#endif //ALIBABANLS_COMMON_TOKEN_H_
