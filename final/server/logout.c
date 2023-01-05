#include "logout.h"

void Logout(void* infoptr)
{
    LOGOUTINFOPTR info = (LOGOUTINFOPTR)infoptr;
    char logInfo[DEFAULT_INFO_LENGTH];

    // user fifo
    char fifo[DEFAULT_FIFO_LENGTH];
    sprintf(fifo, "%s/client_%s", userFifoPre, info->username);

    // change user status
    userNode* u = findUser(info->username, list);
    changeUserStatus(u, list);

    // success
    int fd = openFifoForWR(fifo);
    char buf[RESPONSE] = "Successful Logout!";
    if (write(fd, buf, strlen(buf + 1)) == -1)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to write fifo %s]", fifo);
        SysLog(logInfo);
        #endif  
        close(fd);
        exit(1);
    }
    close(fd);
    #ifdef OPENLOG   
    sprintf(logInfo, "[INFO][LOGOUT][%s][SUCCESS]", info->username);
    SysLog(logInfo);
    UserLog(info->username, logInfo);
    #endif

    // TODO boardcast
    time_t t; time(&t);
    MESSAGEINFO mes;
    mes.response = 0;
    sprintf(mes.sendername, "system");
    sprintf(mes.message, "%s is off line! There are %d users online now.[%s",
    info->username, getOnlineUserNum(list), ctime(&t));
    mes.message[strlen(mes.message) - 1] = ']';

    // send message to all online users
    sendMesToAllOnlineUser(&mes, list);
}