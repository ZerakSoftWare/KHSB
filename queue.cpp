#include "queue.h"

msg_queue_t * create_msg_queue()
{   
    msg_queue_t * s_queue = NULL;

    //s_queue = (msg_queue_t*)malloc(sizeof(msg_queue_t) + 2 * sizeof(LMsgNode_t));
    s_queue = (msg_queue_t*)malloc(sizeof(msg_queue_t));
    
    s_queue->iMsgNumber = 0;
    
    s_queue->m_lpMsgLinkHead = NULL;
    s_queue->m_lpMsgLinkTail = NULL;
    
    pthread_mutex_init(&s_queue->mutex_msg_queue, NULL);
    sem_init(&s_queue->sem_msg_queue, 0, 0);

    return s_queue;
}

int release_msg_queue(msg_queue_t * queue)
{
    if( queue == NULL )
    {
        TRALOG(TRACE_ERROR, "queue == null!");
        return false;
    }

    LMsgNode_t * pTmp = NULL;
    pTmp = queue->m_lpMsgLinkHead;
    while(pTmp != NULL)
    {
        if( queue->m_lpMsgLinkHead == queue->m_lpMsgLinkTail )
        {
            free(queue->m_lpMsgLinkHead->pMessage);
            queue->m_lpMsgLinkHead->pMessage = NULL;
            free(queue->m_lpMsgLinkHead);
            queue->m_lpMsgLinkHead = NULL;
            queue->m_lpMsgLinkTail = NULL;
            
            sem_init(&queue->sem_msg_queue, 0, 0);
        }
        else
        {   
            LMsgNode_t * pNode = queue->m_lpMsgLinkHead;
            queue->m_lpMsgLinkHead = queue->m_lpMsgLinkHead->pNext;
            free(pNode->pMessage);
            pNode->pMessage = NULL;
            free(pNode);
            pNode = NULL;
        }
        queue->iMsgNumber--;
        
        pTmp = pTmp->pNext;
    }

    pthread_mutex_destroy(&queue->mutex_msg_queue);
    sem_destroy(&queue->sem_msg_queue);
    
    free(queue);
    queue = NULL;
    
    return true;
}

int push_message(msg_queue_t * queue, char * msg, int len)
{
    if( queue == NULL )
    {
        TRALOG(TRACE_ERROR, "queue == null!");
        return false;
    }
    
    msg_queue_t * s_queue = queue;

    LMsgNode_t * pNode = (LMsgNode_t *)malloc(sizeof(LMsgNode_t));
    
    pthread_mutex_lock(&s_queue->mutex_msg_queue);

    pNode->pMessage = (char *)malloc(len);
    memcpy((void *)pNode->pMessage, (const void *)msg, len);
    pNode->uiMsgLen = len;
    pNode->pNext = NULL;

    //printf("-1-, pNode->pMessage = %s, pNode->uiMsgLen = %d\n", pNode->pMessage, pNode->uiMsgLen);
    
    if( s_queue->m_lpMsgLinkHead == NULL )
    {
        //printf("-2-, pNode->pMessage = %s, pNode->uiMsgLen = %d\n", pNode->pMessage, pNode->uiMsgLen);
        
        s_queue->m_lpMsgLinkHead = pNode;
        s_queue->m_lpMsgLinkTail = pNode;
    }
    else
    {
        //printf("-3-, pNode->pMessage = %s, pNode->uiMsgLen = %d\n", pNode->pMessage, pNode->uiMsgLen);
        s_queue->m_lpMsgLinkTail->pNext = pNode;
        s_queue->m_lpMsgLinkTail = pNode;
    }
    s_queue->iMsgNumber++;
    //printf("s_queue->iMsgNumber = %d\n", s_queue->iMsgNumber);

    sem_post(&s_queue->sem_msg_queue);
    //printf("%d\n", s_queue->sem_msg_queue);
    

    pthread_mutex_unlock(&s_queue->mutex_msg_queue);
    
    return true;
}

int pop_message(msg_queue_t * queue, char * msg, int * len)
{
    if( queue == NULL )
    {
        TRALOG(TRACE_ERROR, "queue == null!");
        return false;
    }
    
    msg_queue_t * s_queue = queue;
    
    pthread_mutex_lock(&s_queue->mutex_msg_queue);

    memcpy((void *)msg, (const void *)s_queue->m_lpMsgLinkHead->pMessage, s_queue->m_lpMsgLinkHead->uiMsgLen);
    *len = s_queue->m_lpMsgLinkHead->uiMsgLen;

    if( s_queue->m_lpMsgLinkHead == s_queue->m_lpMsgLinkTail )
    {
        free(s_queue->m_lpMsgLinkHead->pMessage);
        s_queue->m_lpMsgLinkHead->pMessage = NULL;
        free(s_queue->m_lpMsgLinkHead);
        s_queue->m_lpMsgLinkHead = NULL;
        s_queue->m_lpMsgLinkTail = NULL;
        
        sem_init(&s_queue->sem_msg_queue, 0, 0);
    }
    else
    {   
        LMsgNode_t * pNode = s_queue->m_lpMsgLinkHead;
        s_queue->m_lpMsgLinkHead = s_queue->m_lpMsgLinkHead->pNext;
        free(pNode->pMessage);
        pNode->pMessage = NULL;
        free(pNode);
        pNode = NULL;
    }
    s_queue->iMsgNumber--;
    //printf("s_queue->iMsgNumber = %d\n", s_queue->iMsgNumber);
    
    pthread_mutex_unlock(&s_queue->mutex_msg_queue);
    
    return true;
}

void printf_queue(msg_queue_t * queue)
{
    LMsgNode_t * pTmp = NULL;
    pTmp = queue->m_lpMsgLinkHead;
    int i = 0;
    while(pTmp != NULL)
    {
        i++;
        //printf("printf_queue: --%d--, [%s:%d:%ld]\n", i, pTmp->pMessage, pTmp->uiMsgLen, queue->sem_msg_queue);
        pTmp = pTmp->pNext;
    }

    return;
}

