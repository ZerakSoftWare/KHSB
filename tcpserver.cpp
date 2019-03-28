#include <arpa/inet.h>
#include <netinet/in.h>

#include "tcpserver.h"

#include "queue.h"
#include "asrcntl.h"
#include "util.h"

char g_listen_ip[255];
int  g_listen_port;
int  gTcpSocket;


int tcp_server_config(char * ipaddress, int port)
{
    strcpy((char *)g_listen_ip, (const char *)ipaddress);
    g_listen_port = port;
    
    return true;
}

int  startup(char* ip,int  port)
{
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd < 0)
    {
        perror("sock");
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip);
    socklen_t len = sizeof(local);

    
    //调高端口的重用性
    int on;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) << 0 )
    {
         perror("setsockopt error\n");
         exit(4);
    }
    
    if( bind(fd,(struct sockaddr*)&local, len) < 0 )
    {
        perror("bind");
        exit(2);
    }
    
    if( listen(fd, 3) < 0)
    {
        perror("listen");
        exit(3);
    }
    return fd;
}


void tcp_get_buff_len(char * buf, int * buflen)
{

#if 0
    char ucLen[4];

    strcpy((char *)ucLen, (const char *)buf + 4);
    *buflen = atoi(ucLen);

    printbuf(ucLen, 4);
    *buflen = ntohl(*buflen);
#else
    msg_header * pheader = (msg_header *)buf;
    *buflen = pheader->msg_len;

    printf("tcp_get_buff_len(), buflen = %d\n", *buflen);
#endif    
    return;
}


int tcp_server_recv()
{
    char ucIPAddress[255];
    int ulPort;

    strcpy((char *)ucIPAddress, g_listen_ip);
    ulPort = g_listen_port;
    
    //接待客人的人
    int fd = startup(ucIPAddress, ulPort);
    printf("g_listen_ip = %s, ulPort = %d, sock: %d\n", g_listen_ip, g_listen_port, fd);
    char recvbuf[MAX_MSG_LEN];
    TcpNlMsg p_tcp_nl_msg;
    
    while(1)
    {
        //接收client套接字的信息
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        //newsock == 服务人员
        gTcpSocket = accept(fd, (struct sockaddr*)&client, &len);
        if(gTcpSocket < 0)
        {
            perror("accept");
            //继续接听下一个
            continue;
        }
        
        //将网络中的数据转换为主机用户可以看懂的数据  
        printf("get new client [%s: %d : %d]\n", inet_ntoa(client.sin_addr),  ntohs(client.sin_port), gTcpSocket);
        
        //1.read   2.write       
        while(1)
        {
            memset((void *)recvbuf, 0, MAX_MSG_LEN);
            int recvlen = read( gTcpSocket, recvbuf, sizeof(recvbuf) - 1);
            //printf("--2--, gTcpSocket = %d\n", gTcpSocket);
            if(recvlen == 0)
            {
                printf("client quit\n");
                break;
            }
            else if( recvlen < 0 )
            {
                //printf("recvlen[%d] < 0", recvlen);
                break;
            }
            
            if (p_tcp_nl_msg.flag_tcp_NL_proc == 1)
            {
                memcpy(p_tcp_nl_msg.g_recv_buff + p_tcp_nl_msg.g_recv_len, recvbuf, recvlen);
                
                recvlen += p_tcp_nl_msg.g_recv_len;
                memcpy(recvbuf, p_tcp_nl_msg.g_recv_buff, recvlen);
            
                p_tcp_nl_msg.flag_tcp_NL_proc = 0;
            }

            char * buf = recvbuf;
            
            while (1)
            {       
                if (recvlen >= sizeof(msg_header))
                {
                    msg_header *head=(msg_header*)buf;
                    uint32_t msglen = head->msg_len;
                    //int type = head->msg_type;
                    
                    if(recvlen >= msglen)
                    {
                        asr_cntl_process((char *)buf);
                        
                        //循环终止条件之一：当前待处理TCP消息恰好为一条完整的应用层消息
                        if (recvlen == msglen)
                        {
                            break;
                        }

                        //printf("recvlen(%u) > msglen(%u)\n", recvlen, msglen);
                        
                        //更新待处理TCP消息缓存和长度，进入下一次while循环
                        recvlen -= msglen;
                        buf += msglen;
                    }
                    else if(recvlen < msglen)
                    {
                        //printf("sizeof(TcpMsgHead):%d < recvlen(%d) < msglen(%u)\n", sizeof(msg_header), recvlen, msglen);

                        memset(p_tcp_nl_msg.g_recv_buff, 0, 2 * MAX_MSG_LEN);
                        memcpy(p_tcp_nl_msg.g_recv_buff, buf, recvlen);
                        p_tcp_nl_msg.g_recv_len = recvlen;
                        p_tcp_nl_msg.flag_tcp_NL_proc = 1;

                        break;
                    }
                }        //代码逻辑流程3.1
                else
                {
                    //printf("recvlen(%u) < sizeof(TcpMsgHead):%u\n", recvlen, sizeof(msg_header));

                    memset(p_tcp_nl_msg.g_recv_buff, 0, MAX_MSG_LEN);
                    memcpy(p_tcp_nl_msg.g_recv_buff, buf, recvlen);
                    p_tcp_nl_msg.g_recv_len = recvlen;
                    p_tcp_nl_msg.flag_tcp_NL_proc = 1;

                    break;
                }

            }
            
            //printf("--3--, gTcpSocket = %d\n", gTcpSocket);
        }
        printf("close gTcpSocket = %d\n", gTcpSocket);
        close(gTcpSocket);
    }
    
    close(fd);
    
    return true;
}

int tcp_server_send(char * buf, int len)
{
    int     nleft = len;
    int     nwrite;
    char *  charbuf = (char *)buf;
    //printf("--1--, len = %d gTcpSocket = %d\n", len, gTcpSocket);

    while(nleft > 0)
    {
        nwrite = write(gTcpSocket, charbuf, nleft);
        //printf("--1--, nwrite = %d\n", nwrite);
        if( nwrite < 0 )
        {
            if(errno == EINTR)
            continue;
            return -1;
        }
        else if(nwrite == 0)
        {
            return len - nleft;
        }
        //printf("--2--, nwrite = %d\n", nwrite);
        charbuf += nwrite;
        nleft = len - nwrite;
        //printf("--3--, nleft = %d\n", nleft);
    }
    
    return len;
}


#if 0

//extern simple_queue * gQueue;
int TcpRecv1()
{
    char ucIPAddress[255];
    int ulPort;

    strcpy((char *)ucIPAddress, g_listen_ip);
    ulPort = g_listen_port;
    
    //接待客人的人
    int listen_sock = startup(ucIPAddress, ulPort);
    printf("g_listen_ip = %s, ulPort = %d, sock: %d\n", g_listen_ip, g_listen_port, listen_sock);
    char buf[1024];
    char tmp[2048];
    int tmplen = 0;
    
    while(1)
    {
        //接收client套接字的信息
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        //newsock == 服务人员
        int newsock = accept(listen_sock, (struct sockaddr*)&client, &len);
        if(newsock < 0)
        {
            perror("accept");
            //继续接听下一个
            continue;
        }
        
        //将网络中的数据转换为主机用户可以看懂的数据  
        printf("get new client [%s: %d]\n", inet_ntoa(client.sin_addr),  ntohs(client.sin_port));
        
        //1.read   2.write       
        int buflen = 0;
        while(1)
        {
            memset((void *)buf, 0, 1024);
            int s = read( newsock, buf, sizeof(buf) - 1);
            if(s > 0)
            {
                buf[s - 1] = '\0';
                printf("client# s = %d \n", s);
                tcp_get_buff_len(buf, &buflen);
                tmplen += s;
                printf("tmplen = %d \n", tmplen);
                if( tmplen < buflen )
                {
                    memcpy((void *)tmp, buf, s);
                    continue;
                }
                buflen += s;
                printf("buflen = %d \n", buflen);
                //if(  )
                
                #if 0
                printf("len = %d\n", s);
                for( int i = 0; i < s; i++)
                {
                    printf("%02x", (unsigned char)buf[i]);
                    if ( i + 1 == s)
                    {
                        printf("\n", buf);
                    }
                }
                #endif
                asr_cntl_process((char *)buf);
                //InsertNode((char *)buf);
                write(newsock, buf, strlen(buf)); //服务器将读到的信息给客户端回显回去
            }
            else if(s == 0)
            {
                printf("client quit\n");
                break;
            }
            else
            {
                break;
            }
        }
        close(newsock);
    }
    
    close(listen_sock);
    
    return true;
}

map<int, TcpNlMsg> g_map_fd_TcpNLMsg;
void RecvTcpMsg(int fd)
{
    map<int, TcpNlMsg>::iterator iter = g_map_fd_TcpNLMsg.find(fd);
    if (iter == g_map_fd_TcpNLMsg.end())
    {
        return;
    }

    TcpNlMsg* p_tcp_nl_msg = &iter->second;

    uint8_t recvbuf[MAX_MSG_LEN];
    memset(recvbuf,0,MAX_MSG_LEN);
    
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    //代码逻辑流程1
    int recvlen = recvfrom(fd, recvbuf, MAX_MSG_LEN, 0, (struct sockaddr *) &src_addr, &addrlen);
    
    if(recvlen == 0)
    {
        printf("uehandle_recv_pack() receive shutdown command from peer endpoint\n")
        
        return;
    }

    if (recvlen == -1)
    {
        printf("uehandle_recv_pack() ret -1,errno=%d"\n, errno);
        return;
    }
    //代码逻辑流程2    //代码逻辑流程2.1
    if (p_tcp_nl_msg->flag_tcp_NL_proc == 1)
    {
        memcpy(p_tcp_nl_msg->g_recvbuf+p_tcp_nl_msg->g_recvlen,recvbuf,recvlen);
        
        recvlen += p_tcp_nl_msg->g_recvlen;
        memcpy(recvbuf, p_tcp_nl_msg->g_recvbuf, recvlen);

        p_tcp_nl_msg->flag_tcp_NL_proc = 0;
    }
    
    uint8_t* buf = recvbuf;
    //代码逻辑3
    while (1)
    {        //代码逻辑3.2
        if (recvlen >= sizeof(TcpMsgHead))
        {
            TcpMsgHead *head=(TcpMsgHead*)buf;
            uint32_t msglen = head->msglen;
            int type = head->msgtpe;
                        //代码逻辑流程3.2.2
            if(recvlen >= msglen)
            {
                ProcessTcpMsg(buf+sizeof(TcpMsgHead), type);//业务处理函数，switch...case语句，根据不同的消息类型进入不同的处理分支
                
                if (recvlen == msglen)//循环终止条件之一：当前待处理TCP消息恰好为一条完整的应用层消息
                {
                    break;
                }

                printf("recvlen(%u) > msglen(%u)\n", recvlen, msglen);
                                //更新待处理TCP消息缓存和长度，进入下一次while循环
                recvlen -= msglen;
                buf += msglen;
            }            //代码逻辑流程3.2.1
            else if(recvlen < msglen)
            {
                printf("sizeof(TcpMsgHead):%u < recvlen(%u) < msglen(%u)",sizeof(TcpMsgHead), recvlen, msglen);

                memset(p_tcp_nl_msg->g_recvbuf, 0, MAX_MSG_LEN);
                memcpy(p_tcp_nl_msg->g_recvbuf, buf, recvlen);
                p_tcp_nl_msg->g_recvlen = recvlen;
                p_tcp_nl_msg->flag_tcp_NL_proc = 1;

                break;
            }
        }        //代码逻辑流程3.1
        else
        {
            printf("recvlen(%u) < sizeof(TcpMsgHead):%u\n", recvlen, sizeof(TcpMsgHead));

            memset(p_tcp_nl_msg->g_recvbuf, 0, MAX_MSG_LEN);
            memcpy(p_tcp_nl_msg->g_recvbuf, buf, recvlen);
            p_tcp_nl_msg->g_recvlen = recvlen;
            p_tcp_nl_msg->flag_tcp_NL_proc = 1;

            break;
        }

    }

}
#endif

