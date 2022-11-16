#include "register.h"


void Register(char* myfifo) 
{
	/* while failed to register */
	char username[USER_MXLEN * 2], password[USER_MXLEN * 2], response[RESPLEN];
	while (1)
	{
		system("clear");
		puts("==================== Register ====================");
		printf("Please input your name(don't exceed 25 charaters): ");
		fgets(username, USER_MXLEN * 2, stdin);
		/* check username's length */
		if (strlen(username) > 26)
		{
			printf("Username's length should be less than 25 charaters :(\nreboot register...\n");
			sleep(2);
			continue;			
		}
		if (strlen(username) == 1)
		{
			printf("Username can not be empty :(\nreboot register...\n");
			sleep(2);
			continue;
		}
		printf("Please input your password(don't exceed 25 charaters): ");
		fgets(password, USER_MXLEN * 2, stdin);
		/* check password's length */
		if (strlen(password) > 26) 
		{
			printf("Password's length should be less than 25 charaters :(\nreboot register...\n");
			sleep(2);
			continue;	
		}
		if (strlen(password) == 1) 
		{
			printf("Password can not be empty :(\nreboot register...\n");
			sleep(2);
			continue;
		}
		/* replace \n to \0 */
		username[strlen(username) - 1] = '\0';
		password[strlen(password) - 1] = '\0';

		/* create my fifo */
		sprintf(myfifo, "/tmp/client_fifos/client_%s", username);
		createFIFO(myfifo);
		
		/* creeate register data structure */
		REGISTERINFO info;
		strncpy(info.username, username, strlen(username) + 1);
		strncpy(info.password, password, strlen(password) + 1);
		strncpy(info.fifo, myfifo, strlen(myfifo) + 1);
		
		/* open server fifo for writing only */
		int fd = openFIFOforWR(SERVER_FIFO_NAMES[0]);
		if ( write(fd, &info, sizeof(REGISTERINFO)) == -1 )
		{	
			printf("Failed to write fifo %s\n", SERVER_FIFO_NAMES[0]);
			perror("");
			exit(EXIT_FAILURE);
		}
		
		/* waiting for server's response */
		fd = openFIFOforRDWB(myfifo);
		if ( read(fd, response, RESPLEN) == -1)
		{	
			printf("Failed to read fifo %s\n", myfifo);
			perror("");
			exit(EXIT_FAILURE);
		}
		
		/* Successful or Failure */
		if (response[0] == 'S')
		{
			printf("Sucessful register! (:\n");
			printf("Backing to main page...\n");
			sleep(2);
			break;	
		}
		else 
		{
			printf("Username has existed ):, please try again!\nrebooting register...\n");
			sleep(2);
		}
	} 	
	
}