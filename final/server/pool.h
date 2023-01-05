#ifndef _POOL_
#define _POOL_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define DEFAULT_MIN_THREAD 3
#define DEFAULT_MAX_THREAD 10
#define DEFAULT_QUEUE_SIZE 200

typedef struct Task
{
    void (*function) (void* arg);
    void* arg;
}Task;

typedef struct threadPool
{
    // task queue
    Task* taskQ;
    int queueCapacity;
    int queueSize;
    int queueFront;
    int queueRear;

    // thread 
    pthread_t managerId;
    pthread_t *threadIds;
    int minNum;
    int maxNum;
    int busyNum;
    int liveNum;
    int exitNum;
    
    // lock
    pthread_mutex_t mutexPool;
    pthread_mutex_t mutexBusy;

    // cond
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;

    int shutDown; // 1: destroy 0: not destroy
}threadPool;

// min thread number in the pool
extern int minThreadNum;
// max thread number in the pool
extern int maxThreadNum;

// create thread pool
threadPool* createThreadPool(int min, int max, int capacity);
// destroy thread pool
int threadPoolDestroy(threadPool* pool);
// add task
void threadPoolAddTask(threadPool* pool, void(*func)(void*), void* arg);
// get busy thread number
int threadPoolBusyNum(threadPool* pool);
// get live thread number
int threadPoolLiveNum(threadPool* pool);

// worker and manager function
void* worker(void* arg);
void* manager(void* arg);
void threadExit(threadPool* pool);

#endif

