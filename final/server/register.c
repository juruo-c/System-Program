#include "register.h"

void RegisterFeedback(char* fifo, int reno)
{
    int fd = openFifoForWR(fifo);
    char buf[RESPONSE];
    switch (reno)
    {
        case 1: // username has been used
			sprintf(buf, "Failure!Username has been used!");
            break;
		case 2: // Successful register!
            sprintf(buf, "Successful!");
            break;
    }
	if (write(fd, buf, strlen(buf) + 1) == -1)
    {
		#ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to write fifo %s]", fifo);
        SysLog(logInfo);
        #endif 
        close(fd);
        exit(1);
    }
    close(fd);
}

void Register(void* infoptr)
{
    REGISTERINFOPTR info = (REGISTERINFOPTR)infoptr;
    char logInfo[DEFAULT_INFO_LENGTH];
    
    // user fifo
    char fifo[DEFAULT_FIFO_LENGTH];
    sprintf(fifo, "%s/client_%s", userFifoPre, info->username);

    // check if username is existed
    userNode* u = findUser(info->username, list);
    if (u != NULL) // exist
    {
        RegisterFeedback(fifo, 1);
        #ifdef OPENLOG 
        sprintf(logInfo, "[INFO][REGISTER][%s][FAILURE(username has been existed)]", info->username);
        SysLog(logInfo);
        #endif
        return;
    }

    // add user Node
    if (addUserNode(info->username, info->password, list) == -1)
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to add user to user list]");
        SysLog(logInfo);
        #endif
        exit(1);
    }

    // success and log
    RegisterFeedback(fifo, 2);
    #ifdef OPENLOG
    createUserLog(info->username);
    sprintf(logInfo, "[INFO][REGISTER][%s][SUCCESS]", info->username);
    SysLog(logInfo);
    UserLog(info->username, logInfo);
    #endif

}