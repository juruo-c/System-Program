#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <mysql.h>
#include "fifo.h"
#include "register.h"
#include "login.h"
#include "message.h"

#define OK 0
#define ERROR -1
#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost" 

char FIFO_NAMES[3][100] = 
{
    "/home/zhengyang2020151002/server_fifos/fifo_1", // register fifo
    "/home/zhengyang2020151002/server_fifos/fifo_2", // login fifo
    "/home/zhengyang2020151002/server_fifos/fifo_3" // send message fifo
};

/* link mysql  */
int link_mysql()
{
		
}

/* signals handler */
void handler(int sig)
{
    int i;
    for (i = 0; i < 3; i ++ ) unlink(FIFO_NAMES[i]);
    exit(1);
}

/* failed to register, write reason(erno) to client fifo */
void RegisterFeedback(int fd, int erno)
{
  	switch (erno)
    {
        case 1: // Exception! Could not open fifo for writing only access.  

            break;
        case 2: // Mysql exception

            break;
        case 3: // Successful register!
            char* buf = "Register :Successful!";
            if (write(fd, buf, strlen(buf)) == -1)
            {
                printf("Failed to write fifo %s", fifo);
                perror("");
                close(fd);
                exit(EXIT_FAILURE);
            }
            break;
    }
}

/* process register request in fifo(fd) */
int Register(int fd)
{
    REGISTERINFO info;
    if (read(fd, &info, sizeof(REGISTERINFO)))
    {
        printf("Failed to read fifo %s\n", FIFO_NAMES[0]);
        perror("");
        exit(EXIT_FAILURE);
    }

    /* open client's fifo for writing only */
    int fd = open(fifo, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("Failed to open fifo %s for writing only", fifo);
        perror("");
        RegisterFeedback(1);
    }

    /* check if username is existed */
	MYSQL conn;
	
	
    /* add userinfo(username, password, fifo) into database */
    
    /* successful */ 
    RegisterFeedback(info.fifo, 3);

    close(fd);
}

/* process login request in fifo(fd) */
void Login(int fd)
{

}

/* process send message request in fifo(fd) */
void sendMessage(int fd)
{

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
            return sendMessage(fd);
    }
}

int main()
{
    int i;

    /* handle signals */
    signal(SIGTERM, handler);

    /* create FIFO */
    for (i = 0; i < 3; i ++ ) createFIFO(FIFO_NAMES[i]);

    /* open FIFO for reading */
    int fifoFd[3], mxFd = -1;
    for (i = 0; i < 3; i ++ ) 
    {
        fifoFd[i] = openFIFO(FIFO_NAMES[i]);
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
