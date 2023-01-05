#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "../common/fifo.h"
#include "../common/clientinfo.h"
#include "../server/config.h"
#include "register.h"
#include "login.h"
#define OPENLOG

char myfifo[DEFAULT_FIFO_LENGTH];
char myMessageFifo[DEFAULT_FIFO_LENGTH];

// signals handler
void handler(int sig)
{
    unlink(myfifo);
    unlink(myMessageFifo);
    exit(1);
}

// show choices
void showChoices()
{
	system("clear");
	puts("Welcome! What do you want to do?");
	puts("=================================");
	puts("=========  1 - register =========");
	puts("=========  2 - login    =========");
	puts("=========  3 - exit     =========");
	puts("=================================");
}

int main()
{
    // handle signals
    signal(SIGTERM, handler);
    signal(SIGINT, handler);

    // config
    config();

    // ask user to make choice
    while(1)
    {
        showChoices();
        int choice; scanf("%d", &choice);
        getchar();
        switch(choice)
        {
            case 1:
                Register();
                break;
            case 2:
                Login();
                break;
            case 3:
                puts("Bye!");
                exit(1);
                break;
            default:
                puts("Please choose 1/ 2/ 3!");
                sleep(2);
                system("clear");
        }
    }
}