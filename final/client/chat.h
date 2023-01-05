#ifndef _CLIENT_CHAT_
#define _CLIENT_CHAT_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../common/fifo.h"
#include "../common/clientinfo.h"
#include <time.h>

#define BUFSIZE 1024

extern char myfifo[DEFAULT_FIFO_LENGTH];
extern char myMessageFifo[DEFAULT_FIFO_LENGTH];

/* client's chat function */
void Chat(char* username);

#endif