#include "config.h"

#include <string.h> 
#include <string> 
#include <unistd.h>    
#include <fcntl.h>    
#include <stdio.h>    
#include <stdlib.h>    
#include <stdarg.h>    

#define  MAX_PATH 260    

char g_szConfigPath[MAX_PATH]; 

// 设置ini文件名
int SetPath(char * pIniFileName)
{
    char buf[MAX_PATH];   
    memset(buf, 0, sizeof(buf));   
    GetCurrentPath(buf, (char *)"khsb.ini");
    printf("buf = %s\n", buf);
    strcpy(g_szConfigPath, buf);
    
    return true;
}

//获取当前程序目录    
int GetCurrentPath(char buf[], char *pFileName)   
{   
    char pidfile[64];   
    int bytes;   
    int fd;   
   
    sprintf(pidfile, "/proc/%d/cmdline", getpid());   
   
    fd = open(pidfile, O_RDONLY, 0);   
    bytes = read(fd, buf, 256);
    if(bytes == 0 )
    {
        printf("read path failure\n");
    }
    close(fd);   
    buf[MAX_PATH] = '\0';   
   
    char * p = &buf[strlen(buf)];   
    do    
    {   
        *p = '\0';   
        p--;   
    } 
    while( '/' != *p );   
   
    p++;   
   
    //配置文件目录    
    memcpy(p, pFileName, strlen(pFileName));   
    
    return 0;   
}   



void trim(char *strIn, char *strOut)
{
    int i, j ;

    i = 0;

    j = strlen(strIn) - 1;
 
    while(strIn[i] == ' ' || strIn[i] == '\t')
    ++i;

    while(strIn[j] == ' ' || strIn[j] == '\t')
    --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}


//从INI文件读取字符串类型数据    
char * GetIniKeyString(char *title, char *key)    
{    
    FILE *fp;    
    char szLine[1024];   
    static char tmpstr[1024];   
    static char rltstr[1024];   
    int rtnval;   
    int i = 0;    
    int flag = 0;    
    char *tmp;   
   
    if((fp = fopen(g_szConfigPath, "r")) == NULL)    
    {    
        printf("have no such file! filename = %s\n", g_szConfigPath);   
        return (char *)"";    
    }   
    
    while(!feof(fp))    
    {    
        rtnval = fgetc(fp);    
        if(rtnval == EOF)    
        {    
            break;    
        }    
        else    
        {    
            szLine[i++] = rtnval;    
        }    
        if(rtnval == '\n')    
        {    
            //i--;   
            szLine[--i] = '\0';   
            i = 0;    
            tmp = strchr(szLine, '=');    
   
            if(( tmp != NULL )&&(flag == 1))    
            {    
                if(strstr(szLine, key)!=NULL)    
                {    
                    //注释行    
                    if ('#' == szLine[0])   
                    {   
                    }   
                    //else if ( '//' == szLine[0] && '//' == szLine[1] )
                    else if ( '\/' == szLine[0] && '\/' == szLine[1] )   
                    {   
                           
                    }
                    else   
                    {   
                        //找打key对应变量    
                        strcpy(tmpstr, tmp + 1);
                        trim(tmpstr, rltstr);
                        fclose(fp);
                        //printf("title = %s, key = %s, rltstr = %s\n", title, key, rltstr);
                        return rltstr;
                    }   
                }    
            }   
            else    
            {    
                strcpy(tmpstr,"[");    
                strcat(tmpstr,title);    
                strcat(tmpstr,"]");   
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 )    
                {   
                    //找到title    
                    flag = 1;    
                }   
            }   
        }   
    }   
    fclose(fp);
    
    return (char *)"";    
}   
   
//从INI文件读取整类型数据    
int GetIniKeyInt(char *title, char *key)   
{   
    char pTmpString[1024] = {};

    strcpy((char *)pTmpString, (const char *)GetIniKeyString(title, key));
    
    if( strlen((char *)pTmpString) == 0 )
    {
        //printf("GetIniKeyInt failure! title = %s, key = %s, filename = %s\n", title, key, g_szConfigPath);
        return -1;
    }
    else
    {
        return atoi(pTmpString);   
    }

}   


void testpath()
{
    SetPath((char *)"khsb.ini");

    int iRlt1, iRlt2;   
    char szRlt[32];
    
    iRlt1 = GetIniKeyInt((char *)"1234", (char *)"good");
    iRlt2 = GetIniKeyInt((char *)"1234", (char *)"ftest");
    strcpy(szRlt, GetIniKeyString((char *)"1234", (char *)"bad"));

    printf("iRlt1 = %d, iRlt2 = %d, szRlt = %s \n", iRlt1, iRlt2, szRlt);
    
    return;
}

