#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TOTAL_LOOP 5
#define THREAD_NUM 4
#define FILE_NUM 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int fileTurn[FILE_NUM];
char fileName[FILE_NUM][10];
int fileTime[FILE_NUM];

void Lock()
{
    if (pthread_mutex_lock(&mutex))
    {
        printf("failed to lock\n");
        exit(EXIT_FAILURE);
    }
}

void Unlock()
{
    if (pthread_mutex_unlock(&mutex))
    {
        printf("failed to unlock\n");
        exit(EXIT_FAILURE);
    }
}

void* func(void* parm)
{
    int num = *((int*) parm);

    int i = 0, j, k;
    while (i < TOTAL_LOOP * FILE_NUM)
    {
        for (j = 0; j < FILE_NUM; j ++ )
        {
            Lock();
            if (i < TOTAL_LOOP * FILE_NUM && fileTurn[j] == num
                && fileTime[j] < TOTAL_LOOP * FILE_NUM)
            {
                // write to file
                int fd = open(fileName[j], O_CREAT | O_APPEND | O_WRONLY, 0600);
                char* buf = (char*) malloc(sizeof(char));
                *buf = num + 1 + '0';
                for (k = 0; k < num + 1; k ++ )
                    write(fd, buf, 1);
                free(buf);
                close(fd);

                fileTurn[j] = (fileTurn[j] + 1) % THREAD_NUM;
                fileTime[j] ++;
                i ++;
            }
            Unlock();
        }
    }
    
}

int main()
{
    pthread_t threadId[THREAD_NUM];
    int parm[THREAD_NUM];

    int i;
    // create file name and initialize file turn
    for (i = 0; i < FILE_NUM; i ++ )
    {
        sprintf(fileName[i], "%c.txt", 'A' + i);
        fileTurn[i] = i;
    }
    // create thread
    for (i = 0; i < THREAD_NUM; i ++ )
    {
        parm[i] = i;
        if (pthread_create(&threadId[i], NULL, &func, parm + i))
        {
            printf("failed to create thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < THREAD_NUM; i ++ )
        if (pthread_join(threadId[i], NULL))
        {
            printf("failed to join thread %d\n", i);
            exit(EXIT_FAILURE);
        }

    return 0;   
}