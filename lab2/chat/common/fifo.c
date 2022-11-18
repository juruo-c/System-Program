#include "fifo.h"

const char SERVER_FIFO_NAMES[SERVER_FIFONUM][100] =
{
    "/tmp/server_fifos/register", // register fifo
    "/tmp/server_fifos/login", // login fifo
    "/tmp/server_fifos/message" // send message fifo
};

/* create FIFO if necessary */
void createFIFO(const char* FIFO_NAME)
{
    if (access(FIFO_NAME, F_OK) == -1)
    {
        if (mkfifo(FIFO_NAME, 0777) != 0)
        {
            printf("FIFO %s was not created\n", FIFO_NAME);
            perror("");
            exit(EXIT_FAILURE);
        }
    }
}

/* open specific fifo for read only */
int openFIFOforRDWR(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_RDWR);
    if (fd == -1)
    {
        printf("Could not open fifo %s for reading only access\n", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* open specific fifo for read only */
int openFIFOforRD(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("Could not open fifo %s for reading only access\n", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* open specific fifo for write only */
int openFIFOforWR(const char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("Could not open fifo %s for writing only access\n", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* open specific fifo for read only(with block) */
int openFIFOforRDWB(const char* FIFO_NAME)
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
