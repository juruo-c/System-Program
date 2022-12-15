#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    int value;
    char string[128];
}thread_parm_t;

void *threadfunc(void *parm)
{
    thread_parm_t *p = (thread_parm_t *)parm;
    printf("%s, parm = %d\n", p->string, p->value);
    return NULL;
}

void Free(thread_parm_t** p)
{
    if (*p)
    {
        free(*p);
        *p = NULL;
    }
}

int main(int argc, char **argv)
{
    pthread_t thread1, thread2;
    int rc = 0;
    thread_parm_t *parm1 = NULL, *parm2 = NULL;
    printf("Creat a thread attributes object\n");
    parm1 = malloc(sizeof(thread_parm_t));
    if (parm1 == NULL)
    {
        perror("failed to malloc thread_parm_t");
        exit(EXIT_FAILURE);
    }
    parm1->value = 5;
    strcpy(parm1->string, "Inside first thread");
    rc = pthread_create(&thread1, NULL, threadfunc, (void*) parm1);
    if (rc != 0)
    {
        printf("failed to create thread1\n");
        Free(&parm1);
        exit(EXIT_FAILURE);
    }

    parm2 = malloc(sizeof(thread_parm_t));
    parm2->value = 77;
    strcpy(parm2->string, "Inside the second thread");
    rc = pthread_create(&thread2, NULL, threadfunc, (void*) parm2);
    if (rc != 0)
    {
        printf("failed to create thread2\n");
        Free(&parm1); Free(&parm2);
        exit(EXIT_FAILURE);
    }

    if (pthread_join(thread1, NULL))
    {
        printf("failed to join thread1\n");
        Free(&parm1); Free(&parm2);
        exit(EXIT_FAILURE);
    }
    if (pthread_join(thread2, NULL))
    {
        printf("failed to join thread2\n");
        Free(&parm1); Free(&parm2);
        exit(EXIT_FAILURE);
    }
    Free(&parm1);
    Free(&parm2);

    printf("Main completed\n");
    return 0;
}