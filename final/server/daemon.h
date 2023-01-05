#ifndef _DAEMON_
#define _DAEMON_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

/*
    create daemon process 
    return values:
        0: success
        1: failure, fork error
*/
int createDaemon();

#endif