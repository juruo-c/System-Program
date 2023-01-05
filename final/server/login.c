#include "login.h"

void LoginFeedback(char* fifo, int reno, int time)
{
    int fd = openFifoForWR(fifo);
    char buf[RESPONSE];
	switch(reno)
	{
		case 1: // username not exist
			sprintf(buf, "Failure! Username doesn't exist.");
			break;
		case 2: // password not correct
			sprintf(buf, "Failure! Password was wrong.");
			break;
		case 3: // user has been online
			sprintf(buf, "Failure! User has been online in other terminal.");
			break;
        case 4: // user has been lock
            sprintf(buf, "Failure! User has been locked, please retry after %d minutes.", time);
            break;
		case 5:	// successful login
			sprintf(buf, "Successful!");
	}		
	if ( write(fd, buf, strlen(buf) + 1) == -1 ) 
	{
		#ifdef OPENLOG
        char logInfo[DEFAULT_INFO_LENGTH];
        sprintf(logInfo, "[ERROR][failed to write fifo %s]", fifo);
        SysLog(logInfo);
        #endif  
		close(fd);
		exit(EXIT_FAILURE);
	}
    close(fd);
}

void Login(void* infoptr)
{
    LOGININFOPTR info = (LOGININFOPTR)infoptr;
    char logInfo[DEFAULT_INFO_LENGTH];

    // user fifo
    char fifo[DEFAULT_FIFO_LENGTH];
    sprintf(fifo, "%s/client_%s", userFifoPre, info->username);

    // check if username is existed
    userNode* u = findUser(info->username, list);
    if (u == NULL) // not existed
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[INFO][LOGIN][%s][FAILURE(username was not existed)]", info->username);
        SysLog(logInfo);
        #endif
        LoginFeedback(fifo, 1, 0);
        return;
    }

    // check if user has been lock
    if (getLoginLock(u, list)) // locked
    {
        // check if the locked status can be change
        time_t now, last = getLoginTime(u, list);
        time(&now);
        if (now - last >= lockTime * 60) // change locked status to be unlocked
        {
            unlockLogin(u, list);
        }
        else 
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[INFO][LOGIN][%s][FAILURE(user has been locked)]", info->username);
            SysLog(logInfo);
            UserLog(info->username, logInfo);
            #endif  
            LoginFeedback(fifo, 4, (lockTime * 60 - (now - last)) / 60);
            return;
        }
    }

    // check if password is right
    if (strcmp(info->password, u->password)) // wrong 
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[INFO][LOGIN][%s][FAILURE(password was wrong)]", info->username);
        SysLog(logInfo);
        UserLog(info->username, logInfo);
        #endif
        failToLogin(u, list);
        LoginFeedback(fifo, 2, 0);
        return;
    }

    // check if user has been online
    if (getUserStatus(u, list) == ONLINE) // online
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[INFO][LOGIN][%s][FAILURE(user has been online)]", info->username);
        SysLog(logInfo);
        UserLog(info->username, logInfo);
        #endif
        LoginFeedback(fifo, 3, 0);
        return;
    }

    // success
    changeUserStatus(u, list);
    LoginFeedback(fifo, 5, 0);
    #ifdef OPENLOG
    sprintf(logInfo, "[INFO][LOGIN][%s][SUCCESS]", info->username);
    SysLog(logInfo);
    UserLog(info->username, logInfo);
    #endif 

    // TODO boardcast 
    time_t t; time(&t);
    MESSAGEINFO mes;
    mes.response = 0;
    sprintf(mes.sendername, "system");
    sprintf(mes.message, "%s is on line! There are %d users online now.[%s",
    info->username, getOnlineUserNum(list), ctime(&t));
    mes.message[strlen(mes.message) - 1] = ']';

    // send message to all online users
    sendMesToAllOnlineUser(&mes, list);

    // TODO send message to this user
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

    // select all messages that are sended to this user when he(she) was offline
    char query[MESSAGE];
    sprintf(query, "select * from message where receiver = '%s';", info->username);
    if (mysql_query(&conn, query))
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to query database]");
        SysLog(logInfo);
        #endif
        exit(1); 
    }
    // for all message, write to message fifo
    strncpy(mes.receivername, info->username, strlen(info->username) + 1);
    MYSQL_RES* res = mysql_store_result(&conn);
    MYSQL_ROW row;
    int fd = openFifoForWR(commonFifos[2]);
    while (row = mysql_fetch_row(res))
    {
        strncpy(mes.sendername, row[0], strlen(row[0]) + 1);
        strncpy(mes.message, row[2], strlen(row[2]) + 1);
        if (write(fd, &mes, sizeof(MESSAGEINFO)) == -1)
        {
            #ifdef OPENLOG
            sprintf(logInfo, "[ERROR][failed to write fifo %s]", commonFifos[2]);
            SysLog(logInfo);
            #endif
            close(fd);
            exit(1); 
        }
    }
    close(fd);

    // delete the message
    sprintf(query, "delete from message where receiver = '%s';", info->username);
    if (mysql_query(&conn, query))
    {
        #ifdef OPENLOG
        sprintf(logInfo, "[ERROR][failed to query database]");
        SysLog(logInfo);
        #endif
        exit(1); 
    }
}