#include "util.h"

void printbuf(char * buf, int len)
{
    int i = 0;

    printf("enter printfbuf, len = %d\n", len);
    
    while(1)
    {
        if( i == len )
        {
            break;
        }
        
        printf("%02x ", (unsigned char)buf[i]);
        i++;

        if( (i % 16) == 0 )
        {
            printf("\n");
        }
    }
    
    return;
}

