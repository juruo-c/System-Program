#include "message.h"

void MessageFeedback(char* fifo, int reno, char* username)
{
    int fd = openFifoForWR(fifo);
    char buf[RESPONSE];
    switch (reno)
    {
        case 1: // username has been used
			sprintf(buf, "Send message to %s successfully!", username);
            break;
		case 2: // Successful register!
            sprintf(buf, "User %s is offline, the message will be sended when he(she) is online.", username);
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

void Message(void* infoptr)
{
    MESSAGEINFOPTR info = (MESSAGEINFOPTR)infoptr;
    char logInfo[DEFAULT_INFO_LENGTH];

    // user fifo
    char fifo[DEFAULT_FIFO_LENGTH];
    char messageFifo[DEFAULT_FIFO_LENGTH];
    sprintf(fifo, "%s/client_%s", userFifoPre, info->sendername);
    sprintf(messageFifo, "%s/client_%s_message", userFifoPre, info->receivername);

    // judge if the receiver is online
    userNode* u = findUser(info->receivername, list);
    if (getUserStatus(u, list) == ONLINE)
    {
        // open fifo for writing
        int fd = openFifoForWRWB(messageFifo);

        if ( write(fd, info, sizeof(MESSAGEINFO)) == -1 )
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][failed to write fifo %s]", messageFifo);
            SysLog(logInfo);
            #endif
            close(fd);
            exit(1);
        }
        close(fd);

        // success
        if (info->response)
            MessageFeedback(fifo, 1, info->receivername);
        #ifdef OPENLOG 
        sprintf(logInfo, "[INFO][MSG][sender: %s][receiver: %s][true]", info->sendername, info->receivername);
        SysLog(logInfo);
        if (info->response) // not system message
            UserLog(info->sendername, logInfo);
        UserLog(info->receivername, logInfo);
        #endif

    }
    else // user is offline, save message to database
    {
        // link database
        MYSQL conn;
        if (mysql_init(&conn) == NULL)
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][Failed to initialize mysql object]");
            SysLog(logInfo);
            #endif
            exit(1);
        }
        if (mysql_real_connect(&conn, _DATABASE_HOST_, _DATABASE_USER_, _DATABASE_PASSWD_, _DATABASE_NAME_, 0, NULL, 0) == NULL)
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][Failed to connect with database]");
            SysLog(logInfo);
            #endif
            exit(1);
        }

        // store message into database
        char query[MESSAGE];
        sprintf(query, "insert into message(sender, receiver, mes)values('%s', '%s', \"%s\");",
        info->sendername, info->receivername, info->message);
        if (mysql_query(&conn, query))
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][failed to query database]", query);
            SysLog(logInfo); 
            #endif
            exit(1);
        }

        // success
        if (info->response)
            MessageFeedback(fifo, 2, info->receivername);
        #ifdef OPENLOG 
        sprintf(logInfo, "[INFO][MSG][sender: %s][receiver: %s][false]", info->sendername, info->receivername);
        SysLog(logInfo);
        if (info->response)
            UserLog(info->sendername, logInfo);
        UserLog(info->receivername, logInfo);
        #endif
    }

}