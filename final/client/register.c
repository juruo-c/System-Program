#include "register.h"

void Register()
{
    char username[USERNAME];
    char password[PASSWORD];
    char response[RESPONSE];
    // while failed to register
    while (1)
    {
        system("clear");
        puts("==================== Register ====================");
        printf("Please input your name(don't exceed 25 charaters): ");
        fgets(username, USERNAME, stdin);
        // check username's length
        if (strlen(username) > USERNAME / 2 + 1)
        {
            while (getchar() != '\n');
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
        // replace \n to \0
        username[strlen(username) - 1] = '\0';
		password[strlen(password) - 1] = '\0';

        // create my fifo
        sprintf(myfifo, "%s/client_%s", userFifoPre, username);
        sprintf(myMessageFifo, "%s/client_%s_message", userFifoPre, username);
        createFifo(myfifo);
        createFifo(myMessageFifo);

        // create register data structure
        REGISTERINFO info;
        strncpy(info.username, username, strlen(username) + 1);
		strncpy(info.password, password, strlen(password) + 1);
		
        // open server fifo for writing only
        int fd = openFifoForWR(commonFifos[0]);
        if (write(fd, &info, sizeof(REGISTERINFO)) == -1)
        {
            printf("Failed to write fifo %s\n", commonFifos[0]);
			perror("");
			close(fd);
            exit(1);
        }
        close(fd);

        // waiting for server's response
        fd = openFifoForRDWB(myfifo);
        if (read(fd, response, RESPONSE) == -1)
        {
            printf("Failed to read fifo %s\n", myfifo);
			perror("");
			close(fd);
			exit(1);
        }
        close(fd);

        // Successful or Failure
        if (response[0] == 'S')
		{
			printf("Successful register! (:\n");
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