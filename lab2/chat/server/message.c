#include "message.h"

/* process send message request in fifo(fd) */
int Message(int fd)
{
	MESSAGEINFO info;
	
	if (read(fd, &info, sizeof(MESSAGEINFO)) == -1)
	{
		printf("Failed to read fifo\n");
		perror("");
		exit(EXIT_FAILURE);
		close(fd);
	}	

	printf("Receive Message! Sender: %s, Receiver: %s, Message: %s\n", info.sendername, info.receivername, info.message);

	/* create receiver's fifo */
	char fifo[100];
	sprintf(fifo, "/tmp/client_fifos/client_%s", info.receivername);
	
	/* open fifo for write */
	fd = openFIFOforWR(fifo);
	if (write(fd, &info, sizeof(MESSAGEINFO)) == -1)
	{
		printf("Failed to write fifo %s\n", fifo);
		perror("");
		exit(EXIT_FAILURE);
		close(fd);
	}
	close(fd);

	/*  */

	return 0;
}

