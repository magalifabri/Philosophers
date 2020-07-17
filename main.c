#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *my_turn(void *arg)
{
	while(1)
	{
		sleep(1);
		printf("my turn!\n");
	}
	return (NULL);
}

void your_turn()
{
	while(1)
	{
		sleep(2);
		printf("your turn!\n");
	}
}

int main()
{
	pthread_t new_thread;

	pthread_create(&new_thread, NULL, my_turn, NULL);
	// my_turn();
	your_turn();
	return (0);
}