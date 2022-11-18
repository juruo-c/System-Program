#ifndef _LOGIN_
#define _LOGIN_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost"

int Login(int fd);

#endif
