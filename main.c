#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()
#include "philosophers.h"

typedef struct s_frk
{
	int available;
	pthread_mutex_t lock;
} t_frk;

typedef struct s_phi
{
	// int forks;
	long long time_last_meal;
	int n_times_eaten;
	// int returned;
} t_phi;

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	t_frk *forks;
	t_phi *phis;
	int phi_died;
} t_tab;

void *phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;

	int phi_n = -1; // sentinel value for an uninitialized philosopher
	int left_fork_held;
	int right_fork_held;
	int phi_state;
	long long time_last_meal;
	long long time_sleep_start;
	struct timeval tp;
	long long cur_tp;

	// LIFE of a philosipher
	while(1)
	{
		// GET current time for time stamp
		gettimeofday(&tp, 0);
		cur_tp = tp.tv_sec;
		cur_tp *= 1000;
		cur_tp += (tp.tv_usec / 1000);
		
		// INITIALIZE philosopher parameters
		if (phi_n == -1)
		{
			phi_n = tab->phi_n;
			left_fork_held = 0;
			right_fork_held = 0;
			phi_state = 's'; // 's' = sleep, 't' = thinking, 'e' = eating
			time_sleep_start = cur_tp;
			time_last_meal = cur_tp; // let's be nice and assume the philo's start on a full stomach
		}

// TEST -----------------------------------------------------------------------
		// if (phi_n == 1)
		// 	tab->phis[1].n_times_eaten = 2;
		// printf("fork availability: %d, %d, %d\n", tab->forks[0].available, tab->forks[1].available, tab->forks[2].available);
// ----------------------------------------------------------------------------

		// DETERMINE activity
		// DEATH, if time_to_die has elapsed
		if (time_last_meal + tab->time_to_die <= cur_tp)
		{
			printf("%lld, %d, died\n", (cur_tp - tab->start_tp), phi_n);
			tab->phi_died = 1;
			if (right_fork_held)
			{
				pthread_mutex_lock(&tab->forks[phi_n].lock);
				tab->forks[phi_n].available = 1;
				pthread_mutex_unlock(&tab->forks[phi_n].lock);
				right_fork_held = 0;
			}
			if (left_fork_held)
			{
				if (phi_n == 0)
				{
					pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock);
					tab->forks[tab->number_of_philosophers - 1].available = 1;
					pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock);
				}
				else
				{
					pthread_mutex_lock(&tab->forks[phi_n - 1].lock);
					tab->forks[phi_n - 1].available = 1;
					pthread_mutex_unlock(&tab->forks[phi_n - 1].lock);
				}
				left_fork_held = 0;
			}
			return (NULL);
		}

		// THINKING: grab forks if available 
		if (phi_state == 't')
		{
			pthread_mutex_lock(&tab->forks[phi_n].lock);
			if (tab->forks[phi_n].available == 1)
			{
				tab->forks[phi_n].available = 0;
				right_fork_held = 1;
				printf("%lld, %d has taken a fork in his right hand\n", (cur_tp - tab->start_tp), phi_n);
			}
			pthread_mutex_unlock(&tab->forks[phi_n].lock);
			if (phi_n == 0)
			{
				pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock);
				if (tab->forks[tab->number_of_philosophers - 1].available == 1)
				{
					tab->forks[tab->number_of_philosophers - 1].available = 0;
					left_fork_held = 1;
					printf("%lld, %d has taken a fork in his left hand\n", (cur_tp - tab->start_tp), phi_n);
				}
				pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock);
			}
			else
			{
				pthread_mutex_lock(&tab->forks[phi_n - 1].lock);
				if (tab->forks[phi_n - 1].available == 1)
				{
					tab->forks[phi_n - 1].available = 0;
					left_fork_held++;
					printf("%lld, %d has taken a fork in his left hand\n", (cur_tp - tab->start_tp), phi_n);
				}
				pthread_mutex_unlock(&tab->forks[phi_n - 1].lock);
			}
			// EATING, if 2 forks are in the philosophers posession: 
			if (left_fork_held && right_fork_held)
			{
				phi_state = 'e';
				time_last_meal = cur_tp;
				printf("%lld, %d has two forks and is nomming the spaghetti\n", (cur_tp - tab->start_tp), phi_n);
			}
		}

		// EATING -> sleeping, if time_to_eat has elapsed
		if (phi_state == 'e' && time_last_meal + tab->time_to_eat <= cur_tp)
		{
			pthread_mutex_lock(&tab->forks[phi_n].lock);
			tab->forks[phi_n].available = 1;
			pthread_mutex_unlock(&tab->forks[phi_n].lock);
			right_fork_held = 0;
			if (phi_n == 0)
			{
				pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock);
				tab->forks[tab->number_of_philosophers - 1].available = 1;
				pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock);
			}
			else
			{
				pthread_mutex_lock(&tab->forks[phi_n - 1].lock);
				tab->forks[phi_n - 1].available = 1;
				pthread_mutex_unlock(&tab->forks[phi_n - 1].lock);
			}
			left_fork_held = 0;
			// check if philo has eaten enough times yet
			tab->phis[phi_n].n_times_eaten++;
			if (tab->number_of_times_each_philosopher_must_eat != -1
			&& tab->phis[phi_n].n_times_eaten
			>= tab->number_of_times_each_philosopher_must_eat)
			{
				printf("%lld, %d has fattened up enough\n", (cur_tp - tab->start_tp), phi_n);
				return (NULL);
			}
			phi_state = 's';
			time_sleep_start = cur_tp;
			printf("%lld, %d finished eating -> going to bed\n", (cur_tp - tab->start_tp), phi_n);
		}

		// SLEEPING -> thinking, if time_to_sleep has elapsed
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < cur_tp)
		{
			phi_state = 't';
			printf("%lld, %d woke up -> Thinker's pose on the toilet\n", (cur_tp - tab->start_tp), phi_n);
		}
		usleep(5000);
	}
	return (NULL);
}

int philo_one(t_tab tab)
{
	tab.phi_died = 0;

	// CREATE fork struct & initialize locks
	tab.forks = malloc(sizeof(t_frk) * tab.number_of_philosophers);
	int n = -1;
	while (++n < tab.number_of_philosophers)
	{
		pthread_mutex_init(&tab.forks[n].lock, NULL);
		tab.forks[n].available = 1;
	}

	// CREATE & initialize philosophers
	tab.phis = malloc(sizeof(t_phi) * tab.number_of_philosophers);
	n = -1;
	while (++n < tab.number_of_philosophers)
	{
		// tab.phis[n].forks = 0;
		tab.phis[n].n_times_eaten = 0;
	}

	// GET starting time for time stamp
	struct timeval tp;
	gettimeofday(&tp, 0);
	tab.start_tp = tp.tv_sec;
	tab.start_tp *= 1000;
	tab.start_tp += (tp.tv_usec / 1000);

	// CREATE threads / start philosophers
	pthread_t *phi_t;
	phi_t = malloc(sizeof(pthread_t) * tab.number_of_philosophers + 1);
	phi_t[tab.number_of_philosophers] = NULL;
	void *i = 0;
	while ((int)i < tab.number_of_philosophers)
	{
		tab.phi_n = (int)i;
		pthread_create(&phi_t[(int)i], NULL, phi_f, &tab);
		i++;
		usleep(5000); // tab.phi_n needs to be copied over in each phi_f thread, so we can only create threads as quickly as phi_f can copy
	}

	int returned = 0;
	while (returned < tab.number_of_philosophers)
	{
		n = 0;
		returned = 0;
		usleep(100);
		if (tab.phi_died)
		{
			write(1, B_RED"A philosopher has starved! Game over.\n\033[0m"RESET, 50);
			return (0);
		}
		while (n < tab.number_of_philosophers)
		{
			if (tab.phis[n].n_times_eaten == tab.number_of_times_each_philosopher_must_eat)
				returned++;
			if (returned == tab.number_of_philosophers)
			{
				write(1, B_GREEN"They're all fat. Welcome to America!\n"RESET, 49);
				return (0);
			}
			n++;
		}
	}
	return (0);
}

int main(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		printf("error: too few or too many arguments\n");
		return (0);
	}

	t_tab tab;
	tab.number_of_philosophers = atoi(av[1]);
	printf("number_of_philosophers: %d\n", tab.number_of_philosophers);
	tab.time_to_die = atoi(av[2]);
	printf("time_to_die: %d milliseconds\n", tab.time_to_die);
	tab.time_to_eat = atoi(av[3]);
	printf("time_to_eat: %d milliseconds\n", tab.time_to_eat);
	tab.time_to_sleep = atoi(av[4]);
	printf("time_to_sleep: %d milliseconds\n", tab.time_to_sleep);
	if (ac == 6)
		tab.number_of_times_each_philosopher_must_eat = atoi(av[5]);
	else
		tab.number_of_times_each_philosopher_must_eat = -1; // sentinel value for absence of value
	printf("number_of_times_each_philosopher_must_eat: %d\n", tab.number_of_times_each_philosopher_must_eat);

	philo_one(tab);
	return (0);
}