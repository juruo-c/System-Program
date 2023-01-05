#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "../server/log.h"

/*
    from file fd read a line and put it into buf
    (at most n - 1 characters, one space for '\0')
    return values:
        0: success
        -1: error
        1: end of file
*/
int getLine(int fd, char* buf, int n);

/*
    transform letter to 0 ~ 35
    return values:
        'a' ~ 'z' -> 0 ~ 25
        '0' ~ '9' -> 26 ~ 35
*/
int charToInt(char ch);

/*
    lock mutex
*/
void Lock(pthread_mutex_t* m);

/*
    unlock mutex
*/
void Unlock(pthread_mutex_t* m);

#endif 