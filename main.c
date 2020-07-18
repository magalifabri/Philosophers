#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	char *state; // 'e' = eating, 's' = sleeping, 't' = thinking, 'i' = idle
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int available_forks;
} t_tab;

void *phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;
	int phi_n = -1; // sentinel value for an uninitialized philosopher
	while(1)
	{
		// initialize philosopher
		if (phi_n == -1)
		{
			phi_n = tab->phi_n;
			tab->state[phi_n] = 'i';
		}
		sleep(1);

		// get current time for time stamp
		struct timeval tp;
		gettimeofday(&tp, 0);
		long long cur_tp;
		cur_tp = tp.tv_sec;
		cur_tp *= 1000;
		cur_tp += (tp.tv_usec / 1000);

		printf("%lld, %c, %d\n", (cur_tp - tab->start_tp), phi_n, tab->state[phi_n]);
	}
	return (NULL);
}

int main()
{
	t_tab tab;

	// initialize struct values
	tab.number_of_philosophers = 3;
	tab.time_to_die = 16000;
	tab.time_to_eat = 1000;
	tab.time_to_sleep = 8000; // 8000ms = 1s
	tab.state = malloc(tab.number_of_philosophers + 1);
	tab.state[tab.number_of_philosophers] = '\0';
	tab.available_forks = tab.number_of_philosophers;

	// get starting time for time stamp
	struct timeval tp;
	gettimeofday(&tp, 0);
	printf("tp: %ld.%d\n", tp.tv_sec, tp.tv_usec);

	tab.start_tp = tp.tv_sec;
	tab.start_tp *= 1000;
	tab.start_tp += (tp.tv_usec / 1000);
	printf("start_tp: %lld\n", tab.start_tp);

	// create threads / start philosophers
	pthread_t *phi_t;
	phi_t = malloc(sizeof(pthread_t) * tab.number_of_philosophers + 1);
	phi_t[tab.number_of_philosophers] = NULL;
	void *i = 0;
	while ((int)i < tab.number_of_philosophers)
	{
		sleep(1); // can only create threads as quickly as the phi_f function processes them
		tab.phi_n = (int)i;
		pthread_create(&phi_t[(int)i], NULL, phi_f, &tab);
		i++;
	}
	pthread_join(*phi_t, NULL);
	return (0);
}