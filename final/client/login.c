#include "login.h"

void Logout(char* username)
{
	// open server fifo for write only
	int fd = openFifoForWR(commonFifos[3]); 
	
	// create logout data structure
	LOGOUTINFO info;
	strncpy(info.username, username, strlen(username) + 1);
	
	// write logout info to server fifo
	if (write(fd, &info, sizeof(LOGOUTINFO)) == -1)
	{
		printf("Failed to write fifo %s\n", commonFifos[3]);
		perror("");
		close(fd);
		exit(1);
	}
	
	close(fd);

    puts("waiting for server's response");
	
	// open client fifo to wait
	fd = openFifoForRDWB(myfifo);
	char response[RESPONSE];
	if (read(fd, response, RESPONSE) == -1)
	{
		printf("Failed to read fifo %s\n", myfifo);
		perror("");
		close(fd);
		exit(1);
	}
	close(fd);

	printf("%s\n", response);
	printf("Backing to main page...\n");
	sleep(2);
}

void monitorMessage()
{
    int fd;
    MESSAGEINFO info;

    while (1)
    {
        fd = openFifoForRDWB(myMessageFifo);
        if (read(fd, &info, sizeof(MESSAGEINFO)) == -1)
        {
            printf("Failed to read fifo %s\n", myfifo);
			perror("");
			close(fd);
            exit(1);
        }

        printf("%s: %s\n", info.sendername, info.message);
        close(fd);
    }
}

void LoginSuccessful(char* username)
{
    pid_t childPid;
    if ((childPid = fork()) == 0) 
        monitorMessage();

	// show user main page and wait user to make choice
	while (1)
	{
		puts("======================================================");
		puts("====================== Welcome! ======================");
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
				Chat(username);
				break;
			case 2:
                kill(childPid, SIGKILL);
                wait(NULL);
				Logout(username);
				return;		
			default :
				printf("Please choose 1/ 2!");
				sleep(2);
		}
	}
}

void Login()
{
    char username[USERNAME];
    char password[PASSWORD];
    char response[RESPONSE];
    // while failed to login
    while (1)
    {
        system("clear");
        puts("==================== Login ====================");
        printf("Please input your name(don't exceed 25 charaters): ");
        fgets(username, USERNAME, stdin);
        // check username's length
        if (strlen(username) > USERNAME / 2 + 1)
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
        fgets(password, PASSWORD, stdin);
        // check password's length
        if (strlen(password) > PASSWORD / 2 + 1)
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
    
        // create my fifo
        sprintf(myfifo, "%s/client_%s", userFifoPre, username);
        sprintf(myMessageFifo, "%s/client_%s_message", userFifoPre, username);
        createFifo(myfifo);
        createFifo(myMessageFifo);

        // create login data structure
        LOGININFO info;
        strncpy(info.username, username, strlen(username) + 1);
        strncpy(info.password, password, strlen(password) + 1);
		
        /* open server fifo for writing only */
        int fd = openFifoForWR(commonFifos[1]);
        if ( write(fd, &info, sizeof(LOGININFO)) == -1 )
        {
            printf("Failed to write fifo %s\n", commonFifos[1]);
            perror("");
            close(fd);
            exit(1);
        }
        close(fd);

        puts("waiting for server's response");

        // waiting for server's response
        fd = openFifoForRDWB(myfifo);
        if ( read(fd, response, RESPONSE) == -1)
        {
            printf("Failed to read fifo %s\n", myfifo);
            perror("");
			close(fd);
            exit(1);
        }
		close(fd);

        // success or failure
        if (response[0] == 'S')
        {
            printf("Successful login! (:\n");
			sleep(2);
			LoginSuccessful(username);
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