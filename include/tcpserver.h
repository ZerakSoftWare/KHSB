#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

typedef struct msg_header_t
{
    int msg_type;
    int msg_len;
}msg_header;

#define MAX_MSG_LEN 65535
typedef struct TCP_NL_MSG_t
{
    //��flag_in_NL_procΪ1ʱ��ǰ�������ֶβ���Ч
    unsigned char g_recv_buff[2*MAX_MSG_LEN];
    
    //ǰ���ۻ�δ�����TCP��Ϣ����
    int g_recv_len;         

    //���ڱ��ǰ���Ƿ��в�������TCP��Ϣ��1����ʾ�У�0����ʾû�У����У���ǰ�������TCP��Ϣ=ǰ���ۻ�δ�����TCP��Ϣ+��ǰ����recvfrom()���յ���TCP��Ϣ����Ϊ0����ǰ�������TCP��Ϣ��Ϊ�½��յ�TCP��Ϣ
    int flag_tcp_NL_proc;
    
}TcpNlMsg;



int tcp_server_config(char * ipaddress, int port);
int tcp_server_recv();
int tcp_server_send(char * buf, int len);


#endif
