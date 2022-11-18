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
#define SERVER_FIFONUM 3
#define USER_MXLEN 25
#define RESPLEN 100

extern const char SERVER_FIFO_NAMES[SERVER_FIFONUM][100];
void createFIFO(const char* FIFO_NAME);
int openFIFOforRD(const char* FIFO_NAME);
int openFIFOforWR(const char* FIFO_NAME);
int openFIFOforRDWB(const char* FIFO_NAME);
int openFIFOforRDWR(const char* FIFO_NAME);

#endif
