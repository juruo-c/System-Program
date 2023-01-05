#ifndef _FIFO_
#define _FIFO_

#define DEFAULT_FIFO_NUMBER 5
#define DEFAULT_FIFO_LENGTH 100

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../server/log.h"

/*
    fifos' name to numbers
*/
enum FIFO 
{
    REGISTER,
    LOGIN,
    MSG,
    LOGOUT,
    USERINFO
};

/*
    fifos' name
    (TODO remember to free)
    user fifo name's prefix
    fifo's fd
*/
extern char commonFifos[DEFAULT_FIFO_NUMBER][DEFAULT_FIFO_LENGTH];
extern char userFifoPre[DEFAULT_FIFO_LENGTH];
extern int fifoFd[DEFAULT_FIFO_NUMBER];

/*
    create all CommonFifos (if not exists)
*/
void createCommonFifos();

/*
    open all CommonFifos
*/
void openCommonFifos();

/*
    create fifo if necessary
*/
void createFifo(char* fifo);

/* 
    open specific fifo for reading and writing 
    return values:
        fifo's fd
*/
int openFifoForRDWR(char* fifo);
/* open specific fifo for reading only*/
int openFifoForRD(const char* FIFO_NAME);
/* open specific fifo for writing only*/
int openFifoForWR(const char* FIFO_NAME);
/* open specific fifo for reading only(with block) */
int openFifoForRDWB(const char* FIFO_NAME);
/* open specific fifo for writing only(with block) */
int openFifoForWRWB(const char* FIFO_NAME);

#endif