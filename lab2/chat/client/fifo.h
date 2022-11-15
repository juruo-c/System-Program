#ifndef _FIFO_
#define _FIFO_
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <mysql.h>

/* const char SERVER_FIFO_NAMES[3][100] =
{
    "/tmp/server_fifos/register", // register fifo
    "/tmp/server_fifos/login", // login fifo
    "/tmp/server_fifos/message" // send message fifo
};*/

void createFIFO(char* FIFO_NAME);
int openFIFOforRD(char* FIFO_NAME);
int openFIFOforWR(char* FIFO_NAME);

#endif
