#ifndef _GETUSERLIST_
#define _GETUSERLIST_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#include "userlist.h"

extern userList* list;

/* process get userlist request */
void UserList(void* usernameptr);

#endif