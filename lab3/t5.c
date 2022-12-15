#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define TOTAL_LOOP_NUM 5
#define THREAD_NUM 4

int isMyTurn = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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
    if (pthread_cond_broadcast(&cond))
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
    int* p = (int*) parm;
    int num = *p;
    int i, j;
    for (i = 0; i < TOTAL_LOOP_NUM; i ++ )
    {
        Lock();
        while (isMyTurn != num) Wait();
        printf("%x", (unsigned int)pthread_self());
        for (j = 0; j < num + 1; j ++ )
            printf("*");
        if (num == THREAD_NUM - 1) printf("\n");
        isMyTurn = (isMyTurn + 1) % THREAD_NUM;
        Unlock();
        Signal();
    }
}

int main()
{
    pthread_t threadId[THREAD_NUM];
    int parm[THREAD_NUM];
    int i;
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

    for (i = 0; i < THREAD_NUM; i ++ )
        printf("thread %d's Id: %x\n", i + 1, threadId[i]);

    return 0;
}
