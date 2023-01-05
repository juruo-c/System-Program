#include "getuserlist.h"

void UserList(void* usernameptr)
{
    char* username = (char*) usernameptr;
    char logInfo[DEFAULT_INFO_LENGTH];
    
    // get all user's information
    USERLISTPTR info = (USERLISTPTR) malloc(sizeof(USERLIST));
    info->usernum = 0;
    getUserList(info, list, username);

    // user fifo
    char fifo[DEFAULT_FIFO_LENGTH];
    sprintf(fifo, "%s/client_%s", userFifoPre, username);

    // write info to user fifo
    int fd = openFifoForWRWB(fifo);
    if (write(fd, info, sizeof(USERLIST)) == -1)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to write fifo %s]", fifo);
        SysLog(logInfo);
        #endif
        close(fd);
        free(info);
        exit(1);
    }
    close(fd);
    free(info);
}