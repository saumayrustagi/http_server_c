#include <stdio.h>
#include <pthread.h>

int i = 0;
pthread_mutex_t mutex;

void *increment()
{
	for(int j = 0; j < 1000000; ++j)
	{
		// pthread_mutex_lock(&mutex);
		i+=1;
		// pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_t t1;
	pthread_create(&t1, NULL, increment, NULL);
	increment();
	printf("%d\n", i);
	pthread_join(t1, NULL);
	printf("%d\n", i);
	return 0;
}