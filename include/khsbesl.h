#ifndef __ESL_H__
#define __ESL_H__

#include <esl.h>

//#define esl_connect_timeout __declspec(dllimport)

//#ifdef __cplusplus
extern "C"
//#endif
{ 
    class EslClient
    {
        public:
            static void Initial();
            static void Release();

            static int CreateEslSocket();
            static void ReleaseEslSocket(esl_handle_t * phandle);

        public: 
            
            static int RecvEslEvent(esl_handle_t * phandle);
            static int testesl();
            static void print_event(esl_event * event);
            static void process_event(esl_handle_t * handle, esl_event * event);

    };
//#ifdef __cplusplus
}
//#endif

#endif
