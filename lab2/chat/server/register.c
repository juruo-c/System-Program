#include "register.h"

/* failed or succeed to register, write result(reno) to client fifo */
void RegisterFeedback(int fd, int reno)
{
	char buf[50];
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
		printf("Failed to write fifo\n");
        perror("");
        close(fd);
        exit(EXIT_FAILURE);
    }
	
}

/* process register request in fifo(fd) */
int Register(int fd)
{
    REGISTERINFO info;
    if ( read(fd, &info, sizeof(REGISTERINFO)) == -1 )
    {
        printf("Failed to read fifo %s\n", SERVER_FIFO_NAMES[0]);
        perror("");
        exit(EXIT_FAILURE);
    }

	/* open client's fifo for writing only */
	char fifo[100] = "/tmp/client_fifos/client_";
	strcat(fifo, info.username);
   	fd = openFIFOforWR(fifo); 

    /* check if username is existed */
   	
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
//	printf("%s\n", query);	
	if (mysql_query(&conn, query))
	{
		printf("Failed to query '%s'\n", query);
		close(fd);
		exit(EXIT_FAILURE);
	}
	res = mysql_store_result(&conn);
	if (mysql_num_rows(res))
	{
		RegisterFeedback(fd, 1);
		mysql_free_result(res);
		close(fd);	
		return -1;
	}	
	mysql_free_result(res);

    /* add userinfo(username, password, fifo) into database */
	memset(query, 0, sizeof(query));
	strcat(query, "Insert into userinfo(name, passwd, online) values('");
	strcat(query, info.username);
	strcat(query, "','");
	strcat(query, info.password);
	strcat(query, "',0);");
	if (mysql_query(&conn, query))
	{
		printf("Failed to query '%s'\n", query);
		close(fd);
		exit(EXIT_FAILURE);
	}
	
    /* successful */
    RegisterFeedback(fd, 2);

    close(fd);
	return 0;
}
