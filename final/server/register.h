#ifndef _REGISTER_
#define _REGISTER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#include "../common/clientinfo.h"
#include "../common/fifo.h"
#include "userlist.h"

extern userList* list;

/*
    deal with register request
*/
void Register(void* infoptr);

/*
    failed or succeed to register, write result(reno) to client fifo
*/
void RegisterFeedback(char* fifo, int reno);

#endif