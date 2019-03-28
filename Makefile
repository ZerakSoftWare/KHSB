DIR_INC = ./include -I./aliasrinclude -I./eslinclude 
DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin
DIR_LIB = ./lib

SRC = $(wildcard ${DIR_SRC}/*.cpp)  
OBJ = $(patsubst %.cpp, ${DIR_OBJ}/%.o, $(notdir ${SRC})) 

TARGET = khsb

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -g -Wall -Wno-write-strings -I${DIR_INC}

SYSEXLIB = -lnlsCppSdk -lnlsCommonSdk -lopus -lcurl -lssl -lcrypto -luuid -lpthread -lm
#STATICLIB = /root/freeswitch-1.6.6/libs/esl/.libs/libesl.a
STATICLIB =  ${DIR_LIB}/libesl.a

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ) $(SYSEXLIB) -o $@ $(STATICLIB)
    
${DIR_OBJ}/%.o:${DIR_SRC}/%.cpp
	$(CC) $(CFLAGS) $(SYSEXLIB) -c $< -o $@ 

.PHONY:clean

clean:
	find ${DIR_OBJ} -name *.o -exec rm -rf {} \;
	find ${DIR_BIN} -name ${TARGET} -exec rm -rf {} \;
