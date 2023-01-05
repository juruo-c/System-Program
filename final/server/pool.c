#include "pool.h"

const int NUMBER = 2;

int minThreadNum = DEFAULT_MIN_THREAD;
int maxThreadNum = DEFAULT_MAX_THREAD;

threadPool* createThreadPool(int min, int max, int capacity)
{
	threadPool* pool = (threadPool*) malloc(sizeof(threadPool));
    if (pool == NULL) return NULL;
    do
    {
        pool->threadIds = (pthread_t*) malloc(sizeof(pthread_t)*max);
        if (pool->threadIds == NULL) break;
        memset(pool->threadIds, 0, sizeof(pthread_t)*max);
        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;
        pool->exitNum = 0;
        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
            break;
        pool->taskQ = (Task*) malloc(sizeof(Task) * capacity);
        pool->queueCapacity = capacity;
        pool->queueSize = pool->queueFront = pool->queueRear = 0;
        pool->shutDown = 0;
        pthread_create(&pool->managerId, NULL, &manager, pool);
        int i;
        for (i = 0; i < min; i ++ )
            pthread_create(&pool->threadIds[i], NULL, &worker, pool);
        return pool;
    }while(0);

    if (pool && pool->threadIds) free(pool->threadIds), pool->threadIds = NULL;
    if (pool && pool->taskQ) free(pool->taskQ), pool->taskQ = NULL;
    if (pool) free(pool), pool = NULL;
    return NULL;
}

void* worker(void* arg)
{
    threadPool* pool = (threadPool*) arg;
    while(1)
    {
        pthread_mutex_lock(&pool->mutexPool);
        while (pool->queueSize == 0 && !pool->shutDown)
        {
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
            // judge if go to die
            if (pool->exitNum > 0)
            {
                pool->exitNum --;
				if (pool->liveNum > pool->minNum)
				{
					pool->liveNum --;
                	pthread_mutex_unlock(&pool->mutexPool);
                	threadExit(pool);
				}
            }

        }

        if (pool->shutDown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }

        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize --;
		pthread_cond_signal(&pool->notFull);
        pthread_mutex_unlock(&pool->mutexPool);

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum ++;
        pthread_mutex_unlock(&pool->mutexBusy);

        task.function(task.arg);
        free(task.arg);
        task.arg = NULL;

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum --;
        pthread_mutex_unlock(&pool->mutexBusy);
	}
    return NULL;
}

void* manager(void* arg)
{
	threadPool* pool = (threadPool*) arg;
    while (!pool->shutDown)
    {
        sleep(3);

        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        // add thread
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int i;
            int counter = 0;
            for (i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i ++ )
            {
                if (pool->threadIds[i] == 0)
                {
                    pthread_create(&pool->threadIds[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum ++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // destroy thread
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);

            int i;
            for (i = 0; i < NUMBER;i ++ )
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }

    }
	
    return NULL;
}

void threadExit(threadPool* pool)
{
    pthread_t tid = pthread_self();
    int i;
    for (i = 0; i < pool->maxNum; i ++ )
    {
        if (pool->threadIds[i] == tid)
        {
            pool->threadIds[i] = 0;
            break;
        }
    }
	printf("thread %ld exit\n", pthread_self());
	pthread_exit(NULL);
}

void threadPoolAddTask(threadPool* pool, void(*func)(void*), void* arg)
{
	pthread_mutex_lock(&pool->mutexPool);
	while (pool->queueSize == pool->queueCapacity && !pool->shutDown)
	{
		// block producer thread
		pthread_cond_wait(&pool->notFull, &pool->mutexPool);
	}
	if (pool->shutDown)
	{
		pthread_mutex_unlock(&pool->mutexPool);
		return;
	}

	// add task
	pool->taskQ[pool->queueRear].function = func;
	pool->taskQ[pool->queueRear].arg = arg;
	pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
	pool->queueSize ++;
	
	pthread_cond_signal(&pool->notEmpty);
	pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(threadPool* pool)
{
	pthread_mutex_lock(&pool->mutexBusy);
	int busyNum = pool->busyNum;
	pthread_mutex_unlock(&pool->mutexBusy);
	return busyNum;
}

int threadPoolLiveNum(threadPool* pool)
{
	pthread_mutex_lock(&pool->mutexPool);
	int liveNum = pool->liveNum;
	pthread_mutex_unlock(&pool->mutexPool);
	return liveNum;
}

int threadPoolDestroy(threadPool* pool)
{
	if (pool == NULL)
		return -1;
	
	pool->shutDown = 1;
	pthread_join(pool->managerId, NULL);
	int i;
	for (i = 0; i < pool->liveNum; i ++ )
	{
		pthread_cond_signal(&pool->notEmpty);
	}
	for (i = 0; i < pool->maxNum; i ++ )
		if (pool->threadIds[i])
			pthread_join(pool->threadIds[i], NULL);
	if (pool->taskQ) free(pool->taskQ), pool->taskQ = NULL;
	if (pool->threadIds) free(pool->threadIds), pool->threadIds = NULL;
	
	pthread_mutex_destroy(&pool->mutexPool);
	pthread_mutex_destroy(&pool->mutexBusy);
	pthread_cond_destroy(&pool->notEmpty);
	pthread_cond_destroy(&pool->notFull);
	free(pool), pool = NULL;

	return 0;
}
