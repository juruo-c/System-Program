#ifndef _REGISTER_
#define _REGISTER_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mysql.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost"

void RegisterFeedback(int fd, int erno);

int Register(int fd);

#endif
