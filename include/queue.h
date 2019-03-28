#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "log.h"

typedef struct LMsgNode 
{
    unsigned int             uiMsgLen;
    char                   * pMessage;
    struct LMsgNode        * pNext;
    
} LMsgNode_t;


typedef struct msg_queue
{
    pthread_mutex_t mutex_msg_queue;
    sem_t           sem_msg_queue;

    int             iMsgNumber;
    
    LMsgNode_t *    m_lpMsgLinkHead;
    LMsgNode_t *    m_lpMsgLinkTail;

}msg_queue_t;

/* �������� */
msg_queue_t * create_msg_queue();

/* ɾ������ */
int release_msg_queue(msg_queue_t * queue);

/* ���� */
int push_message(msg_queue_t * queue, char * msg, int len);

/* ��� */
int pop_message(msg_queue_t * queue, char * msg, int * len);



void printf_queue(msg_queue_t * queue);


#endif
