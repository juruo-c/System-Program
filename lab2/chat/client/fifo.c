#include "fifo.h"

/* create FIFO if necessary */
void createFIFO(char* FIFO_NAME)
{
    if (access(FIFO_NAME, F_OK) == -1)
    {
        if (mkfifo(FIFO_NAME, 0777) != 0)
        {
            printf("FIFO %s was not created", FIFO_NAME);
            perror("");
            exit(EXIT_FAILURE);
        }
    }
}

/* open specific fifo for read only */
int openFIFOforRD(char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("Could not open fifo %s for reading only access", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}

/* open specific fifo for write only */
int openFIFOforWR(char* FIFO_NAME)
{
    int fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fd == -1)
    {
        printf("Could not open fifo %s for writing only access", FIFO_NAME);
        perror("");
        exit(EXIT_FAILURE);
    }
    return fd;
}
