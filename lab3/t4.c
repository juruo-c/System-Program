#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define TOTAL_LOOP_NUM 5
#define MAIN_LOOP_NUM 5
#define SUB_LOOP_NUM 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int isMainThreadTurn = 0;

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

void Signal()
{
    if (pthread_cond_signal(&cond))
    {
        printf("failed to signal\n");
        exit(EXIT_FAILURE);
    }
}

void Wait()
{
    if (pthread_cond_wait(&cond, &mutex))
    {
        printf("failed to wait\n");
        exit(EXIT_FAILURE);
    }
}

void* func(void* parm)
{
    int i, j;
    for (i = 0; i < TOTAL_LOOP_NUM; i ++ )
    {
        Lock();
        while (isMainThreadTurn) Wait();
        printf("LOOP %d:\n", i + 1);
        for (j = 0; j < SUB_LOOP_NUM; j ++ )
            printf("New Thread\n");
        isMainThreadTurn = 1;
        Unlock();
        Signal();
    }
}

int main()
{
    pthread_t threadId;
    if (pthread_create(&threadId, NULL, &func, NULL))
    {
        printf("failed to create new thread\n");
        exit(EXIT_FAILURE);
    }

    int i, j;
    for (i = 0; i < TOTAL_LOOP_NUM; i ++ )
    {
        Lock();
        while (!isMainThreadTurn) Wait();
        for (j = 0; j < MAIN_LOOP_NUM; j ++ )
            printf("MAIN Thread\n");
        isMainThreadTurn = 0;
        Unlock();
        Signal();
    }

    if (pthread_join(threadId, NULL))
    {
        printf("failed to join\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
