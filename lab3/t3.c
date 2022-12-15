#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NLOOP 5000

int counter;

void *increase(void* vptr);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv)
{
    pthread_t threadIdA, threadIdB;

    if (pthread_create(&threadIdA, NULL, &increase, NULL))
    { 
        printf("failed to create pthread A\n");
        exit(EXIT_FAILURE);
    }       
    if (pthread_create(&threadIdB, NULL, &increase, NULL))
    {      
        printf("failed to create pthread B\n");
        exit(EXIT_FAILURE); 
    } 

    if (pthread_join(threadIdA, NULL)) 
    {
        printf("failed to join pthread A\n");
        exit(EXIT_FAILURE);
    } 
    if (pthread_join(threadIdB, NULL))
    { 
        printf("failed to join pthread B\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *increase(void* vptr)
{
    int i, val;
    for (i = 0; i < NLOOP; i ++ )
    {
        if (pthread_mutex_lock(&mutex))
        {
            printf("failed to lock\n");
            exit(EXIT_FAILURE);
        }
        val = counter;
        printf("%x: %d\n", (unsigned int) pthread_self(), val + 1);
        counter = val + 1;
        if (pthread_mutex_unlock(&mutex))
        {
            printf("failed to unlock\n");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}
