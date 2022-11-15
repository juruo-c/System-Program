#include "register.h"

void Register() 
{
	/* while failed to register */
	char username[USERNAME_MXLEN * 2], password[USERNAME_MXLEN * 2];
	while (1)
	{
		system("clear");
		puts("========== Register ==========");
		printf("Please input your name(don't exceed 25 charater): ");
		fgets(username, USERNAME_MXLEN * 2, stdin);
		if (strlen(username) > 26)
		{
			
		}
	} 
	
}
