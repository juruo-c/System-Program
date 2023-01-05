#include "chat.h"

void printDivision(int mxIdLen, int mxUserLen, int mxStatusLen)
{
	int i;
	printf("+"); for (i = 0; i < mxIdLen; i ++ ) printf("-"); printf("+");
    for (i = 0; i < mxUserLen; i ++ ) printf("-"); printf("+");
    for (i = 0; i < mxStatusLen; i ++ ) printf("-"); printf("+"); puts("");
}

int getNumLen(int num)
{
	int res = 0;
	while (num)
		num /= 10, res++;
	return res;
}

void Chat(char* username)
{
    while(1)
    {
        printf("Loading userlist ... \n");
        // open server fifo to write 
        int fd = openFifoForWR(commonFifos[4]);
        if ( write(fd, username, USERNAME) == -1)
        {
            printf("Failed to write fifo %s\n", commonFifos[4]);
			perror("");
			close(fd);
			exit(1);
        }
        close(fd);
    
        // open client fifo to wait
        fd = openFifoForRDWB(myfifo);
        USERLISTPTR userlist = (USERLISTPTR) malloc(sizeof(USERLIST));

        // read client's fifo
        if ( read(fd, userlist, sizeof(USERLIST)) == -1 )
        {
            printf("Failed to write fifo %s\n", myfifo);
			perror("");
			close(fd);
			exit(1);
        }
        close(fd);

        // show userlist
        // calculate the maximum of usernames' length
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
		// print information
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
	
        // wait user makes choice
        int* chatUserIds = (int*) malloc(sizeof(int) * (userlist->usernum));
        for (i = 0; i < userlist->usernum; i ++ )
            chatUserIds[i] = 0;
        char buf[BUFSIZE];
        puts("Please choose one or more user(input id) who you want to send message to (separated by spaces in a line, input nothing to refresh the userlist)");
        fgets(buf, BUFSIZE, stdin);
        int flag = 0;
        for (i = 0; buf[i]; i ++)
        {
            if (buf[i] == ' ' || buf[i] == '\n') continue;
            if (buf[i] < '0' || buf[i] > '9')
            {
                puts("Contains illegal character(s), please retry.");
                sleep(2);
                flag = 1;
                break;
            }
            j = i;
            while (buf[j + 1] && buf[j + 1] != ' ' && buf[j + 1] != '\n') j ++;

            int id = 0, k;
            for (k = i; k <= j; k ++ )
                id = id * 10 + buf[k] - '0';
            if (id < 1 || id > userlist->usernum)
            {
                printf("User id should be number between 1 to %d\n", userlist->usernum);
                sleep(2);
                flag = 1;
                break;
            }
            flag = 2;
            
            chatUserIds[id - 1] = 1;
            i = j;
        }

        if (flag != 2)
        {
            free(userlist);
            free(chatUserIds);
            system("clear");
            continue;
        }

        //  wait user to input message
        char message[MESSAGE];
        puts("Please input your message (no more than 200 letters, press ENTER to stop)");
        fgets(message, MESSAGE, stdin);
        if (strlen(message) > MESSAGE / 2 + 1)
		{
			while (getchar() != '\n');
			puts("Failed to send message! Message's length should be less than 200 charaters :(\n");
			system("clear");
            free(userlist);
            free(chatUserIds);
            continue;
		}	
        if (strlen(message) == 1)
		{
			puts("Failed to send message! Message should not be empty :(\n");
			system("clear");
            free(userlist);
            free(chatUserIds);
            continue;
		}
        message[strlen(message) - 1] = '\0';

        // write message information to server
        time_t t; time(&t);
        MESSAGEINFO info;
        info.response = 1;
        strncpy(info.sendername, username, strlen(username) + 1);
        sprintf(info.message, "%s[%s", message, ctime(&t));
        info.message[strlen(info.message) - 1] = ']';
        char response[RESPONSE];
        for (i = 0; i < userlist->usernum; i ++ )
            if (chatUserIds[i])
            {   
                strncpy(info.receivername, userlist->usernames[i], strlen(userlist->usernames[i]) + 1);
                fd = openFifoForWR(commonFifos[2]);
                if (write(fd, &info, sizeof(MESSAGEINFO)) == -1)
                {
                    printf("Failed to write fifo %s\n", commonFifos[2]);
                    perror("");
                    free(userlist);
                    free(chatUserIds);
                    close(fd);
                    exit(1);
                }
                close(fd);

                fd = openFifoForRDWB(myfifo);
                if (read(fd, response, RESPONSE) == -1)
                {
                    printf("Failed to read fifo %s\n", myfifo);
                    perror("");
                    free(userlist);
                    free(chatUserIds);
                    close(fd);
                    exit(1);
                }
                close(fd);

                printf("%s\n", response);
            }

        sleep(2);
        free(userlist);
        free(chatUserIds);
        break;
    }
}