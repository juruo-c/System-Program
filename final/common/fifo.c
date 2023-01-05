#include "fifo.h"

char commonFifos[DEFAULT_FIFO_NUMBER][DEFAULT_FIFO_LENGTH];
int fifoFd[DEFAULT_FIFO_NUMBER];
char userFifoPre[DEFAULT_FIFO_LENGTH];

void createCommonFifos()
{
    int i;
    for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
        createFifo(commonFifos[i]);
}

void openCommonFifos()
{
    int i;
    for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
        fifoFd[i] = openFifoForRDWR(commonFifos[i]);
}

void createFifo(char* fifo)
{
    if (access(fifo, F_OK) == -1)
    {
        if (mkfifo(fifo, 0666) != 0)
        {
            #ifdef OPENLOG
            char logInfo[DEFAULT_INFO_LENGTH];
            sprintf(logInfo, "[ERROR][failed to create fifo %s]", fifo);
            SysLog(logInfo);
            #endif
            exit(1);
        }
    }
}

int openFifoForRDWR(char* fifo)
{
    int fd = open(fifo, O_RDWR);
    if (fd == -1)
    {
        #ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to open fifo %s for RDWR]", fifo);
        SysLog(logInfo);
        #endif
        exit(1);
    }
    return fd;
}

int openFifoForWR(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        #ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to open fifo %s for WR only]", FIFO_NAME);
        SysLog(logInfo);
        sprintf(logInfo, "%s", strerror(errno));
        SysLog(logInfo);
        #endif
        exit(1);
    }
    return fd;
}

int openFifoForRD(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        #ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to open fifo %s for RD only]", FIFO_NAME);
        SysLog(logInfo);
        sprintf(logInfo, "%s", strerror(errno));
        SysLog(logInfo);
        #endif
        exit(1);
    }
    return fd;
}

int openFifoForWRWB(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1)
    {
        #ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to open fifo %s for WR only(with block)]", FIFO_NAME);
        SysLog(logInfo);
        sprintf(logInfo, "%s", strerror(errno));
        SysLog(logInfo);
        #endif
        exit(1);
    }
    return fd;
}

int openFifoForRDWB(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1)
    {
        printf("Could not open fifo %s for reading only access\n", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}