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
    //当flag_in_NL_proc为1时，前面两个字段才有效
    unsigned char g_recv_buff[2*MAX_MSG_LEN];
    
    //前次累积未处理的TCP消息长度
    int g_recv_len;         

    //用于标记前次是否有不完整的TCP消息，1，表示有；0，表示没有；若有，当前待处理的TCP消息=前次累积未处理的TCP消息+当前利用recvfrom()接收的新TCP消息，若为0，则当前待处理的TCP消息即为新接收的TCP消息
    int flag_tcp_NL_proc;
    
}TcpNlMsg;



int tcp_server_config(char * ipaddress, int port);
int tcp_server_recv();
int tcp_server_send(char * buf, int len);


#endif
