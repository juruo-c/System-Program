#include "userlist.h"

int UserList(int fd)
{
	/* read username from server fifo */
	char username[USERNAME];
	if (read(fd, username, USER_MXLEN * 2) == -1)
	{
		printf("Failed to read fifo\n");
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	} 
	
	/* get userlist from database */
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
	
	/* get all user except the user whose name is 'username' */
	char query[200]; sprintf(query, "select * from userinfo where name <> '%s';", username);
	if (mysql_query(&conn, query))
	{
		printf("Failed to query '%s'\n", query);
		close(fd);
		exit(EXIT_FAILURE);
	}	
	MYSQL_RES* res;
	MYSQL_ROW row;
	res = mysql_store_result(&conn);

	USERLISTPTR info = (USERLISTPTR) malloc(sizeof(USERLIST));
	info->usernum = 0;
	while (row = mysql_fetch_row(res))
	{
		if (strcmp(username, row[1]))
		{
			strncpy(info->usernames[info->usernum], row[1], strlen(row[1]) + 1);
			if (strcmp("1", row[3])) info->userstatus[info->usernum] = 0;
			else info->userstatus[info->usernum] = 1;
			info->usernum ++;
		}
	}			
	/* open client's fifo for writing only */
	char fifo[100] = "/tmp/client_fifos/client_";
	strcat(fifo, username);
	fd = openFIFOforWR(fifo);

	/* write info to client's fifo */
	if (write(fd, info, sizeof(USERLIST)) == -1)
	{
		printf("Failed to write fifo %s\n", fifo);
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	}	
	
	close(fd);
	free(info);
	return 0;
}
