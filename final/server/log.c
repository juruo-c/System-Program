#include "log.h"

char logPath[DEFAULT_LOGPATH_LENGTH];
char sysLogPath[DEFAULT_LOGPATH_LENGTH];

void UserLog(char* username, char* info)
{
    char userLogPath[DEFAULT_LOGPATH_LENGTH];
    char Info[DEFAULT_INFO_LENGTH];

    // add cur time at the end
    time_t* curTime = malloc(sizeof(time_t));
    time(curTime);
    sprintf(Info, "%s[%s", info, ctime(curTime));
    Info[strlen(Info) - 1] = ']';
    Info[strlen(Info) + 1] = '\0';
    Info[strlen(Info)] = '\n';
    free(curTime);
    
    // write to user log file
    sprintf(userLogPath, "%s/%s.log", logPath, username);
    int fd = open(userLogPath, O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        #ifdef OPENLOG
        char err[DEFAULT_INFO_LENGTH];
        sprintf(err, "[ERROR][failed to open user log %s for writing only]", userLogPath);
        SysLog(err);
        #endif
        exit(1);
    }
    if (write(fd, Info, strlen(Info)) != strlen(Info))
    {
        #ifdef OPENLOG
        char err[DEFAULT_INFO_LENGTH];
        sprintf(err, "[ERROR][failed to write user log %s]", userLogPath);
        SysLog(err);
        #endif
        exit(1);
    }
    close(fd);
}

void SysLog(char* info)
{
    int fd = open(sysLogPath, O_WRONLY | O_APPEND);
    char Info[DEFAULT_INFO_LENGTH];

    // add cur time at the end
    time_t curTime;
    time(&curTime);
    sprintf(Info, "%s[%s", info, ctime(&curTime));
    Info[strlen(Info) - 1] = ']';
    Info[strlen(Info) + 1] = '\0';
    Info[strlen(Info)] = '\n';

    write(fd, Info, strlen(Info));
    close(fd);
}

void createUserLog(char* username)
{
    char path[DEFAULT_LOGPATH_LENGTH];
    sprintf(path, "%s/%s.log", logPath, username);
    if (access(path, F_OK) == -1)
    {
        int fd = creat(path, 0600);
        if (fd == -1)
        {
            printf("log file %s was not created\n", path);
            perror("");
            exit(1);
        } 
        close(fd);
    }
}

void createSysLog()
{
    if (access(sysLogPath, F_OK) == -1)
    {
        int fd = creat(sysLogPath, 0600);
        if (fd == -1)
        {
            printf("log file %s was not created\n", sysLogPath);
            perror("");
            exit(1);
        } 
        close(fd);
    }
}
