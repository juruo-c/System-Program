#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <mysql.h>
#include "fifo.h"
#include "register.h"

#define _DATABASE_NAME_ "test"
#define _DATABASE_USER_ "root"
#define _DATABASE_PASSWD_ "123456"
#define _DATABASE_HOST_ "localhost" 

char myfifo[100];

/* signals handler  */
void handler(int sig)
{
	unlink(myfifo);	
	exit(1);
}

/* show choices */
void showChoices()
{
	puts("Welcome! What do you want to do?");
	puts("=================================");
	puts("=========  1 - register =========");
	puts("=========  2 - login    =========");
	puts("=========  3 - exit     =========");
	puts("=================================");
}

int main()
{
	int i;
	
	/* handle signals */
	signal(SIGTERM, handler);
	
	/* create myfifo */
	sprintf(myfifo, "/tmp/client_fifos/client%d", getpid());
	createFIFO(myfifo);		

	/* ask user to make choice */
	while(1)
	{
		showChoices();	
		int choice; scanf("%d", &choice);
		switch(choice) {
			case 1:
				Register();	
				break;
			case 2:
//				Login();
				break;
			case 3:
				puts("Bye!");
				exit(1);
				break;
			default:
				puts("Please choose 1/ 2/ 3!");	
		}
	}

	return 0;
}
