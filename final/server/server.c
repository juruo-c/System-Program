#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include "config.h"
#include "daemon.h"
#include "log.h"
#include "userlist.h"
#include "pool.h"
#include "register.h"
#include "login.h"
#include "message.h"
#include "logout.h"
#include "getuserlist.h"

userList* list;
threadPool* pool;

void ProcessRequest(int request, int fd)
{
    char logInfo[DEFAULT_INFO_LENGTH];
    switch (request)
    {
        case 0: // register
        {
            REGISTERINFOPTR infoptr = (REGISTERINFOPTR) malloc(sizeof(REGISTERINFO));
            if (read(fd, infoptr, sizeof(REGISTERINFO)) == -1)
            {
                #ifdef OPENLOG
                sprintf(logInfo, "[ERROR][failed to read fifo %s]", commonFifos[0]);
                SysLog(logInfo);
                #endif 
                exit(1);
            }
            threadPoolAddTask(pool, &Register, infoptr);
            break;
        }
        case 1: // login
        {
            LOGININFOPTR infoptr = (LOGININFOPTR) malloc(sizeof(LOGININFO));
            if (read(fd, infoptr, sizeof(LOGININFO)) == -1)
            {
                #ifdef OPENLOG
                sprintf(logInfo, "[ERROR][failed to read fifo %s]", commonFifos[1]);
                SysLog(logInfo);
                #endif
                exit(1);
            }
            threadPoolAddTask(pool, &Login, infoptr);
            break;
        }
        case 2: // send message 
        {
            MESSAGEINFOPTR infoptr = (MESSAGEINFOPTR) malloc(sizeof(MESSAGEINFO));
            if (read(fd, infoptr, sizeof(MESSAGEINFO)) == -1)
            {
                #ifdef OPENLOG
                sprintf(logInfo, "[ERROR][failed to read fifo %s]", commonFifos[2]);
                SysLog(logInfo);
                #endif
                exit(1);
            }
            threadPoolAddTask(pool, &Message, infoptr);
            break;
        }
		case 3: // logout
        {
            LOGOUTINFOPTR infoptr = (LOGOUTINFOPTR) malloc(sizeof(LOGOUTINFO));
            if ( read(fd, infoptr, sizeof(LOGOUTINFO)) == -1 )
            {
                #ifdef OPENLOG
                sprintf(logInfo, "[ERROR][failed to read fifo %s]", commonFifos[3]);
                SysLog(logInfo);
                #endif
                exit(1);
            }
			threadPoolAddTask(pool, &Logout, infoptr);
            break;
        }
		case 4: // return user list
		{
            char* username = (char*) malloc(sizeof(char) * USERNAME);
            if (read(fd, username, USERNAME) == -1)
            {
                #ifdef OPENLOG
                sprintf(logInfo, "[ERROR][failed to read fifo %s]", commonFifos[4]);
                SysLog(logInfo);
                #endif
                exit(1); 
            }
            threadPoolAddTask(pool, &UserList, username);
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    // read configuration file and parse it 
    config();

    // create system log file
    createSysLog(); 
    
    char logInfo[DEFAULT_INFO_LENGTH];
    // create daemon process
    if (createDaemon() == -1)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to fork process when create Daemon]");
        SysLog(logInfo);
        #endif
        exit(1);
    }    
    #ifdef OPENLOG
    sprintf(logInfo, "[INFO][Create Daemon successfully]");       
    SysLog(logInfo);
    #endif

    // create fifos and open them
    createCommonFifos();
    openCommonFifos();
    #ifdef OPENLOG
    sprintf(logInfo, "[INFO][Create and open common fifos successfully]");
    SysLog(logInfo);
    #endif 

    // create userList
    list = createUserList(maxUserNum);
    if (list == NULL)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to create userList]");
        SysLog(logInfo);
        #endif
        exit(1);
    }
    #ifdef OPENLOG
    sprintf(logInfo, "[INFO][Create userList successfully]");
    SysLog(logInfo);
    #endif

    // create thread pool
    pool = createThreadPool(minThreadNum, maxThreadNum, DEFAULT_QUEUE_SIZE);
    if (pool == NULL)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to create thread pool]");
        SysLog(logInfo);
        #endif
        exit(1);
    }
    #ifdef OPENLOG
    sprintf(logInfo, "[INFO][Create thread pool successfully]");
    SysLog(logInfo);
    #endif

    // monitor common fifos
    int mxFd = -1, i;
    for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
        mxFd = (mxFd < fifoFd[i] ? fifoFd[i] : mxFd);
    fd_set readFdSet;
    while (1)
    {
        // init fds' set
        FD_ZERO(&readFdSet);
        for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ ) 
            FD_SET(fifoFd[i], &readFdSet);
        
        // if fds' set changes
        if (select(mxFd + 1, &readFdSet, NULL, NULL, NULL) == -1)
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][failed to select fdSet]");
            SysLog(logInfo);
            #endif
            exit(1);
        }

        // when fds's set changes (get request)
        for (i = 0; i < DEFAULT_FIFO_NUMBER; i ++ )
        {
            if (FD_ISSET(fifoFd[i], &readFdSet))
                ProcessRequest(i, fifoFd[i]);
        }
    }

    return 0;
}