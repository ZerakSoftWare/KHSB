#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEFAULT_CONFIG_FILE "khsb.ini"

int SetPath(char * pIniFileName);
int GetCurrentPath(char buf[], char *pFileName);
char * GetIniKeyString(char *title, char *key);
int GetIniKeyInt(char *title, char *key); 

void testpath();


#endif

