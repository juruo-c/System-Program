#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

int main()
{
	MYSQL conn;
	int res;
	MYSQL_RES* result;
	MYSQL_ROW row;
	mysql_init(&conn);
	
	if (mysql_real_connect(&conn, "localhost", "root", "123456", "test", 0, NULL, 0))
	{
		printf("Connect mysql successful!\nPlease choose the functions.\n");
		int choose;
		char str1[20], str2[20];
		char insert_query[80];
		printf("1.insert\n");
		scanf("%d", &choose);
		printf("Please input your name: ");
		scanf("%s", str1);
		printf("Please input your age: ");
		scanf("%s", str2);
		
		memset(insert_query, 0, sizeof(insert_query));
		strcat(insert_query, "insert into person(name, age) values('");
		strcat(insert_query, str1);
		strcat(insert_query, "',");
		strcat(insert_query, str2);
		strcat(insert_query, ");");
		printf("your SQL: %s\n", insert_query);
		res = mysql_query(&conn, insert_query);
		if (!res)
		{
			printf("insert %lu rows\n", (unsigned long) mysql_affected_rows(&conn));
		}
		else 
		{
			printf("insert error\n");
		} 
		
		mysql_close(&conn);
	}
	return 0;
}
