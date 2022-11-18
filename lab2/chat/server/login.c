#include "login.h"

/* fail or succeed to login, write result(reno) to client fifo */
void LoginFeedback(int fd, int reno)
{
	char buf[100];
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
		case 4:	// successful login
			sprintf(buf, "Successful!");
	}		
	if ( write(fd, buf, strlen(buf) + 1) == -1 ) 
	{
		printf("Failed to write fifo\n");
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	}
}

/* process login request in fifo(fd) */
int Login(int fd)
{
	LOGININFO info;
	if ( read(fd, &info, sizeof(LOGININFO)) == -1 )
	{
        printf("Failed to read fifo %s\n", SERVER_FIFO_NAMES[0]);
        perror("");
        exit(EXIT_FAILURE);
	}	
	
	/* open client's fifo for writing only */
	char fifo[100] = "/tmp/client_fifos/client_";
	strcat(fifo, info.username);
	fd = openFIFOforWR(fifo);
		
    /* check if username is existed, if password is correct */
	/* link database */
    MYSQL conn;
    if (mysql_init(&conn) == NULL)
    {
        printf("Failed to initialize mysql object\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (mysql_real_connect(&conn, _DATABASE_HOST_, _DATABASE_USER_, _DATABASE_PASSWD_, _DATABASE_NAME_, 0, NULL, 0) == NULL)
    {
        printf("Failed to connect with database\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
	
	/* check if username is existed in user table  */
    MYSQL_RES* res;
    MYSQL_ROW row;
    char query[200] = "SELECT * FROM userinfo WHERE name='";
    strcat(query, info.username);
    strcat(query, "';"); 
    if (mysql_query(&conn, query))
    {
        printf("Failed to query '%s'\n", query);
        close(fd);
        exit(EXIT_FAILURE);
    }
    res = mysql_store_result(&conn);
    if (!mysql_num_rows(res)) // username is not existed
    {
        LoginFeedback(fd, 1);
        mysql_free_result(res);
        close(fd);
        return -1;
    }
	
	/* check if password is correct */
	row = mysql_fetch_row(res);
	if ( strcmp(info.password, row[2]) ) // password is wrong
	{
		LoginFeedback(fd, 2);
		mysql_free_result(res);
		close(fd);
		return -1;
	}

	/* check if user has been online */
	if ( !strcmp("1", row[3]) ) // user has been online at other teminal
	{
		LoginFeedback(fd, 3);
		mysql_free_result(res);
		close(fd);
		return -1;
	}

	/* login successfully */
	// change user status to online
	sprintf(query, "update userinfo set online = 1 where name = '");
	strcat(query, info.username);
	strcat(query, "';");
	if (mysql_query(&conn, query))
    {
        printf("Failed to query '%s'\n", query);
        close(fd);
        exit(EXIT_FAILURE);
    }
	// successful
	LoginFeedback(fd, 4);

    mysql_free_result(res);
	close(fd);
	return 0;
}
