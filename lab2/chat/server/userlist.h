#ifndef _USERLIST_
#define _USERLIST_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost"


int UserList(int fd);

#endif
