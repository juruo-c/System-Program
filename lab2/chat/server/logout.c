#include "logout.h"

int Logout(int fd)
{
	LOGOUTINFO info;
	if ( read(fd, &info, sizeof(LOGOUTINFO)) == -1)
	{
		printf("Failed to read fifo %s\n", SERVER_FIFO_NAMES[3]);
		perror("");
		exit(EXIT_FAILURE);
	}
	
	/* open client's fifo for writing only */
	char fifo[100] = "/tmp/client_fifos/client_";
	strcat(fifo, info.username);
	fd = openFIFOforWR(fifo);
	
	/* set uesr's online field to 0 */
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
	
	/* update record */
	char query[200] = "update userinfo set online = 0 where name='";
	strcat(query, info.username);
	strcat(query, "';");
	if (mysql_query(&conn, query))
    {
        printf("Failed to query '%s'\n", query);
        close(fd);
        exit(EXIT_FAILURE);
    }

	/* Logout successfully */
	char buf[100] = "Successful Logout!";
	if (write(fd, buf, strlen(buf) + 1) == -1)
	{
		printf("Failed to write fifo %s\n", fifo);
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	}	

	close(fd);
	return 0;	
}
