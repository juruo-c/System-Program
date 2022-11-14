#ifndef _FIFO_
#define _FIFO_
const char FIFO_NAMES[3][100] =
{
    "/home/zhengyang2020151002/server_fifos/fifo_1", // register fifo
    "/home/zhengyang2020151002/server_fifos/fifo_2", // login fifo
    "/home/zhengyang2020151002/server_fifos/fifo_3" // send message fifo
};

void createFIFO(char* FIFO_NAME);
int openFIFO(char* FIFO_NAME);

#endif
