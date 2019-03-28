#include "khsbesl.h"

#include "log.h"



void EslClient::Initial()
{
    return ;
}

void EslClient::Release()
{
    return ;
}


int EslClient::CreateEslSocket()
{
    esl_handle_t esl_handle;
    esl_status_t esl_status;
    memset(&esl_handle, 0, sizeof(esl_handle));
    
    esl_status = esl_connect(&esl_handle, "127.0.0.1", 8021, NULL, "ClueCon");
    if( esl_status != ESL_SUCCESS )
    {
        TRALOG(TRACE_ERROR, "Connect Error, %d", esl_status);
        return false;
    }
    TRALOG(TRACE_INFO, "Connect Success, %d", esl_status);
    
    return true;
}

void EslClient::ReleaseEslSocket(esl_handle_t * phandle)
{
    esl_disconnect(phandle);
    
    return ;
}



int EslClient::RecvEslEvent(esl_handle_t * phandle)
{
    esl_status_t esl_status;
    esl_status = esl_recv_event(phandle, 1, NULL);
    if( esl_status != ESL_SUCCESS )
    {
        return ESL_FAIL;
    }

    return ESL_SUCCESS;
}


void EslClient::print_event(esl_event * event)
{
    esl_event_header * hp; 

    for (hp = event->headers; hp; hp = hp->next) 
    {
        printf("%s:%s\n", hp->name, hp->value);
    }
    return ;
}

void EslClient::process_event(esl_handle_t * handle, esl_event * event)
{
    const char * uuid;
    const char * caler_uuid;
    
    //switch_core_session_t * other_session = switch_core_session_locate(uuid);

    //const char * uuid;

    //printf("event = %s\n", esl_event_name(event->event_id));

    switch(event->event_id)
    {
        case ESL_EVENT_CHANNEL_PROGRESS_MEDIA:
            printf("event = %s\n", esl_event_name(event->event_id));
            break;
        case ESL_EVENT_CHANNEL_PROGRESS:
            printf("event = %s\n", esl_event_name(event->event_id));
            break;
        case ESL_EVENT_CHANNEL_ANSWER:
            print_event(event);
            uuid = esl_event_get_header(event, "Unique-ID");
            printf("Unique-ID = %s\n", uuid);
            caler_uuid = esl_event_get_header(event, "caller-unique-id");
            printf("caller-unique-id = %s\n", caler_uuid);

#if 0
            esl_event_t *new_event;
            esl_event_create(&new_event, ESL_EVENT_CLONE);
            esl_event_add_header_string(new_event, ESL_STACK_BOTTOM, "11111111", "22222222");
            esl_execute(handle, "stop_asr", NULL, NULL);
#endif      
            //esl_execute(handle, "hangup", NULL, caler_uuid);
            break;
        case ESL_EVENT_CHANNEL_DESTROY:
            print_event(event);
            break;
        case ESL_EVENT_CHANNEL_HANGUP:
            print_event(event);
            break;
        default:
            //print_event(event);
            break;
    }

    return;
}

int EslClient::testesl()
{
    esl_handle_t esl_handle;
    esl_status_t esl_status;
    memset(&esl_handle, 0, sizeof(esl_handle));
    
    esl_status = esl_connect(&esl_handle, "127.0.0.1", 8021, NULL, "ClueCon");
    if( esl_status != ESL_SUCCESS )
    {
        TRALOG(TRACE_ERROR, "Connect Error, %d", esl_status);
        return false;
    }
    TRALOG(TRACE_INFO, "Connect Success, %d", esl_status);
    
    esl_events(&esl_handle, ESL_EVENT_TYPE_PLAIN, "ALL");
        
    while( (esl_status = esl_recv_event(&esl_handle, 1, NULL)) == ESL_SUCCESS )
    {
        if( esl_handle.last_ievent )
        {
            process_event(&esl_handle, esl_handle.last_ievent);
        }
    }

    printf("enter esl_disconnect()\n");
    esl_disconnect(&esl_handle);
    
    return true;
}



