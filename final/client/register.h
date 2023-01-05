#ifndef _CLIENT_REGISTER_
#define _CLIENT_REGISTER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../common/clientinfo.h"
#include "../common/fifo.h"

extern char myfifo[DEFAULT_FIFO_LENGTH];
extern char myMessageFifo[DEFAULT_FIFO_LENGTH];

/* client's register function */
void Register();

#endif