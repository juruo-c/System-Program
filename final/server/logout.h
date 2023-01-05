#ifndef _LOGOUT_
#define _LOGOUT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#include "userlist.h"

extern userList* list;

/* process logout request */
void Logout(void* infoptr);

#endif