#ifndef _LOG_
#define _LOG_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#define OPENLOG

#define DEFAULT_LOGPATH_LENGTH 100
#define DEFAULT_INFO_LENGTH 200

/*
    the log file path prefix 
    the system log file path
*/
extern char logPath[DEFAULT_LOGPATH_LENGTH];
extern char sysLogPath[DEFAULT_LOGPATH_LENGTH];

/*
    create log file for user
*/
void createUserLog(char* username);

/*
    create log file for system(server)
*/
void createSysLog();

/*
    write log information(info) to user(username)'s log file
*/
void UserLog(char* username, char* info);

/*
    write log information(info) to system log file
*/
void SysLog(char* info);


#endif