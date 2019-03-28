#include "test.h"

#include "map.h"

void testaliasr()
{
    printf("enter testaliasr()\n");
    ALIASR * pAliasr = new ALIASR();

    

    delete pAliasr;
    return;
}

void testqueue()
{
    int size = 0;

    size = sizeof(LMsgNode_t);
    printf("size = %d\n", size);

    msg_queue_t * queue = create_msg_queue();

    char ucString1[255] = "123";
    int uclen1 = strlen((const char *)ucString1);
    push_message(queue, ucString1, uclen1);
        
    char ucString2[255] = "4567";
    int uclen2 = strlen((const char *)ucString2);
    push_message(queue, ucString2, uclen2);

    printf_queue( queue);

    char ucStringOut1[255];
    int ucLenOut1 = 0;
    pop_message(queue, ucStringOut1, &ucLenOut1);
    printf("out1[%s:%d]\n", ucStringOut1, ucLenOut1 );

    
    printf_queue( queue);

    char ucStringOut2[255];
    int ucLenOut2 = 0;
    pop_message(queue, ucStringOut2, &ucLenOut2);
    printf("out2[%s:%d]\n", ucStringOut2, ucLenOut2 );
    
    
}


void testmap()
{
    insert_map_relation("a", "aa");
    insert_map_relation("b", "bb");
    insert_map_relation("c", "cc");
    insert_map_relation("d", "dd");
    insert_map_relation("e", "ee");
    insert_map_relation("f", "ff");

    char pOut[255];
    get_map_relation("a", pOut);
    printf("pOut = %s\n", pOut);
    get_map_relation("f", pOut);
    printf("pOut = %s\n", pOut);
    
}
    
