#ifndef _CHAT_
#define _CHAT_

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "../common/fifo.h"
#include "../common/clientinfo.h"

void Chat(char* username, char* myfifo);

#endif
