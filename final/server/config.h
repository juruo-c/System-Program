#ifndef _CONFIG_
#define _CONFIG_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../common/clientinfo.h"
#include "log.h"
#include "../common/utils.h"
#include "../common/fifo.h"
#include "userlist.h"
#include "pool.h"
#define CONFIG_FILE_PATH "config.conf"
#define BUFSIZE 1024


/*
    Read configuration file and parse it
*/
void config();

/*
    copy file name buf[] to commonfile[fileType][]
    return value:
        0: success
        1: failure
*/
int copyFileName(char* buf, enum FIFO fileType);

/*
    check if all chars in buf are digits
    return values:
        1: true
        0: false
*/
int checkIfAllDigit(char* buf);

/*
    parse one line in the configuration file
    return value:
        0: success
        -1: failure
*/
int parseLine(char* line);

#endif