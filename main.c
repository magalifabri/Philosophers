#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

// typedef struct philosopher_state
// {

// }

// void *phi_f(void *arg)
// {
// 	printf("hello, hello\n");
// 	return (NULL);
// }

// int main()
// {
// 	int number_of_philosophers;
// 	int time_to_die;   // time in ms before the next meal needs to start
// 	int time_to_eat;   // duration in ms that the philo will spend eating
// 	int time_to_sleep; // duration in ms that the philosopher will spend sleeping

// 	number_of_philosophers = 2;
// 	time_to_die = 16000;
// 	time_to_eat = 1000;
// 	time_to_sleep = 8000; // 8000ms = 1s

// 	// create philosiphers functions
// 	void (**phi_f)(void *);
// 	phi_f = malloc(sizeof(phi_f) * number_of_philosophers);

// 	// create philosophers
// 	char *phi = malloc(number_of_philosophers + 2);
// 	pthread_t *phi_t = malloc(sizeof(pthread_t) * (number_of_philosophers + 2));
// 	phi[number_of_philosophers + 1] = '\0';
// 	int i = 0;
// 	phi[i] = ' ';
// 	while (++i <= number_of_philosophers)
// 	{
// 		phi[i] = ' ';
// 		pthread_create(&phi_t[i],
// 					   NULL,
// 					   phi_f[i],
// 					   NULL);
// 	}
// }

// void *my_turn(void *arg)
// {
// 	while(1)
// 	{
// 		sleep(1);
// 		printf("my turn!\n");
// 	}
// 	return (NULL);
// }

// void your_turn()
// {
// 	while(1)
// 	{
// 		sleep(2);
// 		printf("your turn!\n");
// 	}
// }

// int main()
// {
// 	pthread_t new_thread;

// 	pthread_create(&new_thread, NULL, my_turn, NULL);
// 	// my_turn();
// 	your_turn();
// 	return (0);
// }

typedef struct s_tab
{
	int phi_n;
	char state;
} t_tab;

void *phi_f(void *arg)
{
	while(1)
	{
		sleep(1);
		printf("%d\n", (int)arg);
	}
	return (NULL);
}

// current state: 3 threads are created and their i index number is passed to phi_f.
int main()
{
	// create threads
	int number_of_philosophers = 3;
	pthread_t *phi_t;
	phi_t = malloc(sizeof(pthread_t) * number_of_philosophers + 1);
	phi_t[number_of_philosophers] = NULL;

	void *i = 0;
	while ((int)i < number_of_philosophers)
	{
		pthread_create(&phi_t[(int)i], NULL, phi_f, i);
		i++;
	}

	pthread_join(*phi_t, NULL);
	return (0);
}