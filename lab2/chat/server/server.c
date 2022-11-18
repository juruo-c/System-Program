#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <mysql.h>
#include "../common/fifo.h"
#include "register.h"
#include "login.h"
#include "message.h"

#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost" 

/* signals handler */
void handler(int sig)
{
    int i;
    for (i = 0; i < 3; i ++ ) unlink(SERVER_FIFO_NAMES[i]);
    exit(1);
}

/* preprocess the request in fifo(fd) */
int ProcessRequest(int request, int fd)
{
    switch (request)
    {
        case 0: // register
            return Register(fd);
        case 1: // login
            return Login(fd);
        case 2: // send message 
            return 0;
    }
}

int main()
{
    int i;

    /* handle signals */
    signal(SIGTERM, handler);

    /* create FIFO */
    for (i = 0; i < 3; i ++ ) createFIFO(SERVER_FIFO_NAMES[i]);

    /* open FIFO for reading */
    int fifoFd[3], mxFd = -1;
    for (i = 0; i < 3; i ++ ) 
    {
        fifoFd[i] = openFIFOforRDWR(SERVER_FIFO_NAMES[i]);
        mxFd = (mxFd < fifoFd[i] ? fifoFd[i] : mxFd);
    }

    /* use select to monitor three server fifos */
    fd_set readFdSet;
    while (1)
    {   
        /* init fds' set */
        FD_ZERO(&readFdSet);
        for (i = 0; i < 3; i ++ ) FD_SET(fifoFd[i], &readFdSet);

        /* if fds' set changes */
        if (select(mxFd + 1, &readFdSet, NULL, NULL, NULL) == -1)
        {
            perror("Failed to select fd set");
            exit(EXIT_FAILURE);
        }

        /* when fds's set changes(get request from client), read fifo */
        for (i = 0; i < 3; i ++ )
        {
            if (FD_ISSET(fifoFd[i], &readFdSet))
                ProcessRequest(i, fifoFd[i]);
        }

    }

    /* close FIFOs */
    for (i = 0; i < 3; i ++ ) close(fifoFd[i]);

    return 0;
}
