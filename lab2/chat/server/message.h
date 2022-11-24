/* process send message request in fifo(fd) */
#ifndef _MESSAGE_
#define _MESSAGE_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../common/fifo.h"
#include "../common/clientinfo.h"

int Message(int fd);

#endif

