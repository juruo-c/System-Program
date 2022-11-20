#include "login.h"

void Logout(char* username, char* myfifo)
{
	/* open server fifo for write only */
	int fd = openFIFOforWR(SERVER_FIFO_NAMES[3]); 
	
	/* create logout data structure */
	LOGOUTINFO info;
	strncpy(info.username, username, strlen(username) + 1);
	
	/* write logout info to server fifo */
	if (write(fd, &info, sizeof(LOGOUTINFO)) == -1)
	{
		printf("Failed to write fifo %s\n", SERVER_FIFO_NAMES[3]);
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	}
	
	close(fd);
	
	/* open client fifo to wait */
	fd = openFIFOforRDWB(myfifo);
	char response[RESPLEN];
	if (read(fd, response, RESPLEN) == -1)
	{
		printf("Failed to read fifo %s\n", myfifo);
		perror("");
		close(fd);
		exit(EXIT_FAILURE);
	}
	close(fd);

	printf("%s\n", response);
	printf("Backing to main page...\n");
	sleep(2);
}

void Login_Successful(char* username, char* myfifo)
{
	/* handle the SIGINT signal */
	// TODO handle SIGINT signal( Logout user )
		
	/* show user main page and wait user to make choice */
	while (1)
	{
		system("clear");
		printf("======================================================\n");
		printf("==================== %s, Welcome! ====================\n", username);
		puts("=============== what do you want to do ===============");
		puts("===============                        ===============");
		puts("=============== 1 - chat with someone  ===============");
		puts("=============== 2 - logout             ===============");
		puts("===============                        ===============");
		puts("======================================================");
		int choice;
		scanf("%d", &choice);		
		switch(choice)
		{
			case 1:
				Chat(username, myfifo);
				break;
			case 2:
				Logout(username, myfifo);
				return;		
			default :
				printf("Please choose 1/ 2!");
				sleep(2);
		}
	}
}

void Login(char* myfifo)
{
	/* while failed to login */
	char username[USER_MXLEN * 2], password[USER_MXLEN * 2], response[RESPLEN];
	while (1) 
	{
		system("clear");
        puts("==================== Login ====================");
        printf("Please input your name(don't exceed 25 charaters): ");
        fgets(username, USER_MXLEN * 2, stdin);
        /* check username's length */
        if (strlen(username) > USER_MXLEN + 1)
        {
			while(getchar() != '\n');
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
        if (strlen(password) > USER_MXLEN + 1)
        {
			while(getchar() != '\n');
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

		/* create login data structure */
		LOGININFO info;
		strncpy(info.username, username, strlen(username) + 1);
        strncpy(info.password, password, strlen(password) + 1);
				
		/* open server fifo for writing only */
        int fd = openFIFOforWR(SERVER_FIFO_NAMES[1]);
        if ( write(fd, &info, sizeof(LOGININFO)) == -1 )
        {
            printf("Failed to write fifo %s\n", SERVER_FIFO_NAMES[1]);
            perror("");
            exit(EXIT_FAILURE);
        }

        /* waiting for server's response */
        fd = openFIFOforRDWB(myfifo);
        if ( read(fd, response, RESPLEN) == -1)
        {
            printf("Failed to read fifo %s\n", myfifo);
            perror("");
			close(fd);
            exit(EXIT_FAILURE);
        }
		close(fd);
		
		/* Successful or Failure */
		if (response[0] == 'S')
		{
			printf("Successful login! (:\n");
			sleep(2);
			Login_Successful(username, myfifo);
			break;
		}
		else 
		{
			printf("%s\n", response);
			printf("rebooting login...\n");
			sleep(2);
		}		
	}
}
