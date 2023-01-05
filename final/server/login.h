#ifndef _LOGIN_
#define _LOGIN_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql.h>
#include "log.h"
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#include "userlist.h"
#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost"

extern userList* list;

/*
    process login request
*/
void Login(void* infoptr);

#endif