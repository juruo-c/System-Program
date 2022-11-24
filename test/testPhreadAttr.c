#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_func(void *arg)
{
	printf("I am fine, and hope you are fine too.\n");
	pthread_exit(EXIT_SUCCESS);
}

int main()
{
	pthread_attr_t attr;
	pthread_t tid;

	int ret;
	ret = pthread_attr_init(&attr);
	if (ret)
	{
		perror("Failed to initialize pthread attr");
		exit(EXIT_FAILURE);
	}

	ret = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
	if (ret)
	{
		perror("Failed to set scope");
		exit(EXIT_FAILURE);
	}

	ret = pthread_create(&tid, &attr, &thread_func, NULL);
	if (ret)
	{
		perror("Failed to create thread");
		exit(EXIT_FAILURE);
	}

	return 0;
}
