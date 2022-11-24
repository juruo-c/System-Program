#include "chat.h"

/* print the division part of userlist */
void printDivision(int mxIdLen, int mxUserLen, int mxStatusLen)
{
	int i;
	printf("+"); for (i = 0; i < mxIdLen; i ++ ) printf("-"); printf("+");
    for (i = 0; i < mxUserLen; i ++ ) printf("-"); printf("+");
    for (i = 0; i < mxStatusLen; i ++ ) printf("-"); printf("+"); puts("");
}

/* return number's decimal length */
int getNumLen(int num)
{
	int res = 0;
	while (num)
		num /= 10, res++;
	return res;
}

/* monitor my fifo to get otheruser's messages */
void monitorMessage(char* username)
{
	int fd;
	MESSAGEINFO info;

	char myfifo[100];
	sprintf(myfifo, "/tmp/client_fifos/client_%s", username);
	
	while (1)
	{
		fd = openFIFOforRDWB(myfifo);

		if (read(fd, &info, sizeof(MESSAGEINFO)) == -1)
		{
			printf("Failed to read fifo %s\n", myfifo);
			perror("");
			close(fd);
			exit(EXIT_FAILURE);
		}

		printf("%s: %s\n", info.sendername, info.message);

		close(fd);
	}
}

void chatWith(char* username, char* myfifo, char* otheruser)
{
	/* clean up stdin */
	while(getchar() != '\n');

	system("clear");

	printf("================ chat with %s ================\n", otheruser);

	/* fork child process to monitor user's fifo for otheruser's message */
	pid_t childpid;
	if ((childpid = fork()) == 0)
		monitorMessage(username);
		
	int fd;
	char message[MASSAGE];
	MESSAGEINFO info;
	strncpy(info.sendername, username, strlen(username) + 1);
	strncpy(info.receivername, otheruser, strlen(otheruser) + 1);	

	while (1)
	{
		printf(">"); fflush(stdout);
		
		fgets(message, MASSAGE, stdin);
		/* check message's length */
		if (strlen(message) > MASSAGE / 2 + 1)
		{
			while (getchar() != '\n');
			puts("Failed to send message! Message's length should be less than 200 charaters :(\n");
			continue;
		}		
		if (strlen(message) == 1)
		{
			puts("Failed to send message! Message should not be empty :(\n");
			continue;
		}
	
		/* copy message to MESSAGEINFO */
		message[strlen(message) - 1] = '\0';
		strncpy(info.message, message, strlen(message) + 1);

		/* check quit; */
		if (strcmp("quit;", message) == 0)
		{
			kill(childpid, SIGKILL);
			wait(NULL);
			break;
		}

		/* open server fifo for write */
		fd = openFIFOforWR(SERVER_FIFO_NAMES[2]);
		if (write(fd, &info, sizeof(MESSAGEINFO)) == -1)
		{
			printf("Failed to write fifo %s\n", SERVER_FIFO_NAMES[2]);
			perror("");
			close(fd);
			exit(EXIT_FAILURE);
		}
		close(fd);
	}
}

void Chat(char* username, char* myfifo)
{
	while (1)
	{
		printf("Loading userlist ...\n");
		/* open server fifo to write */
		int fd = openFIFOforWR(SERVER_FIFO_NAMES[4]);
		if ( write(fd, username, USER_MXLEN * 2) == -1 )
		{
			printf("Failed to write fifo %s\n", SERVER_FIFO_NAMES[4]);
			perror("");
			close(fd);
			exit(EXIT_FAILURE);
		}
		close(fd);
	
		/* open client fifo to wait */
		fd = openFIFOforRDWB(myfifo);
		USERLISTPTR userlist = (USERLISTPTR) malloc(sizeof(USERLIST)); // remember to free
			
		/* read client fifo */
		if ( read(fd, userlist, sizeof(USERLIST)) == -1 )
		{
			printf("Failed to write fifo %s\n", myfifo);
			perror("");
			close(fd);
			exit(EXIT_FAILURE);		
		}
	
		/* show userlist */
		/* calculate the maximum of usernames' length */
		int mxUserLen = 0, i, j, mxStatusLen = 8, mxIdLen = getNumLen(userlist->usernum) + 2;
		int hasoffline = 0;
		for (i = 0; i < userlist->usernum; i ++ )
		{
			int len = strlen(userlist->usernames[i]) + 2;
			mxUserLen = len > mxUserLen ? len : mxUserLen;
			if (userlist->userstatus[i] == 0) hasoffline = 1;	
		}
		mxUserLen = 6 > mxUserLen ? 6 : mxUserLen;
		mxStatusLen += hasoffline;
		mxIdLen = 4 > mxIdLen ? 4 : mxIdLen;
	
		system("clear");
		/* print information */
		printDivision(mxIdLen, mxUserLen, mxStatusLen);
		printf("| id"); for (i = 0; i < mxIdLen - 4; i ++ ) printf(" "); printf(" | ");		
		printf("name"); for (i = 0; i < mxUserLen - 6; i ++) printf(" "); printf(" | ");
		printf("status"); if (hasoffline) printf(" ");  printf(" |"); puts("");
		printDivision(mxIdLen, mxUserLen, mxStatusLen);	
		for (i = 0; i < userlist->usernum; i ++ )
		{
			// id
			printf("| "); for (j = 0; j < mxIdLen - 2 - getNumLen(i + 1); j ++) printf(" "); 
			printf("%d", i + 1);
			// name
			int len = strlen(userlist->usernames[i]);
			printf(" | ");
			printf("%s", userlist->usernames[i]); for (j = 0; j < mxUserLen - len - 2; j ++ ) printf(" ");
			printf(" | ");
			// status
			printf("%s", userlist->userstatus[i] ? "online" : "offline");
			if (hasoffline && userlist->userstatus[i]) printf(" ");
			printf(" |");
			puts("");
		}
	 	printDivision(mxIdLen, mxUserLen, mxStatusLen);
	
		/* wait user makes choice */
		printf("Please choose user(input id) who you want to chat with(input '114514' to refresh userlist, input '9982' to go back to user main page): ");
		fflush(stdout);
		int choice;
		scanf("%d", &choice);
		if (choice == 114514)
		{
			free(userlist);
			close(fd);
			system("clear");
			continue;
		}
		else if(choice == 9982)
		{
			printf("Backing to user main page...\n");
			sleep(2);
			free(userlist);
			close(fd);
			return;
		}
		else if(choice > userlist->usernum || choice <= 0)
		{
			printf("User id should be number between 1 to %d\n", userlist->usernum);
			sleep(2);
			free(userlist);
			close(fd);
			system("clear");
			continue;
		}
		else 
		{	
			if (!userlist->userstatus[choice - 1])
			{
				puts("User who you chat with should be online!");
				sleep(2);
				free(userlist);
				close(fd);
				system("clear");
				continue;	
			}
			chatWith(username, myfifo, userlist->usernames[choice - 1]);			
		}
		/* free userlist and close fd */
		free(userlist);
		close(fd);
	}
}
