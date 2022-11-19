#include "chat.h"

void Chat(char* username, char* myfifo)
{
	/* open server fifo to write */
	int fd = openFIFOforWR(SERVER_FIFO_NAMES[4]);
	if ( write(fd, "1", 2) == -1 )
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
		
	}

	free(userlist);
	close(fd);
}
