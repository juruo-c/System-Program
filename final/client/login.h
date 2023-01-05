#ifndef _CLIENT_LOGIN_
#define _CLIENT_LOGIN_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "../common/fifo.h"
#include "../common/clientinfo.h"
#include "chat.h"

extern char myfifo[DEFAULT_FIFO_LENGTH];
extern char myMessageFifo[DEFAULT_FIFO_LENGTH];

/* client's login function */
void Login();

#endif