#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()

typedef struct s_frk
{
	int available;
	pthread_mutex_t lock;
} t_frk;

typedef struct s_phi
{
	int state;
	int forks;
	long long time_last_meal;
	int n_times_eaten;
} t_phi;

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	char *state; // 'e' = eating, 's' = sleeping, 't' = thinking, 'i' = idle
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	t_frk *forks;
	t_phi *phis;
} t_tab;

void *phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;

	int phi_n = -1; // sentinel value for an uninitialized philosopher
	int left_fork_held;
	int right_fork_held;
	int state;
	long long time_last_meal;
	long long time_sleep_start;

	// LIFE of a philosipher
	while(1)
	{
		// GET current time for time stamp
		struct timeval tp;
		gettimeofday(&tp, 0);
		long long cur_tp;
		cur_tp = tp.tv_sec;
		cur_tp *= 1000;
		cur_tp += (tp.tv_usec / 1000);
		
		// if (phi_n == 0)
		// 	printf("%lld\n", (cur_tp - tab->start_tp));

		// INITIALIZE philosopher parameters
		if (phi_n == -1)
		{
			phi_n = tab->phi_n;
			left_fork_held = 0;
			right_fork_held = 0;
			state = 's';
			time_sleep_start = cur_tp;
			time_last_meal = cur_tp; // let's be nice and assume the philo's start on a full stomach
			// tab->state[phi_n] = 'i';
		}
		// printf("fork availability: %d, %d, %d\n", tab->forks[0].available, tab->forks[1].available, tab->forks[2].available);
		// DETERMINE activity
		// death if time_to_die has elapsed
		if (time_last_meal + tab->time_to_die <= cur_tp)
		{
			printf("%lld, %d, died\n", (cur_tp - tab->start_tp), phi_n);
			if (left_fork_held)
			{
				left_fork_held = 0;
				if (phi_n == 0)
					tab->forks[tab->number_of_philosophers - 1].available = 1;
				else
					tab->forks[phi_n].available = 1;
			}
			if (right_fork_held)
			{
				right_fork_held = 0;
				tab->forks[phi_n - 1].available = 1;
			}
		}

		// thinking -> grab forks if available 
		if (state == 't')
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
			// 2 forks are in the philosophers posession -> eating 
			if (left_fork_held && right_fork_held)
			{
				state = 'e';
				time_last_meal = cur_tp;
				printf("%lld, %d obtained two forks and is attacking the spaghetti\n", (cur_tp - tab->start_tp), phi_n);
			}
		}

		// eating -> sleeping if time_to_eat has elapsed
		if (state == 'e' && time_last_meal + tab->time_to_eat <= cur_tp)
		{
			tab->forks[phi_n].available = 1;
			if (phi_n == 0)
				tab->forks[tab->number_of_philosophers - 1].available = 1;
			else
				tab->forks[phi_n - 1].available = 1;
			left_fork_held = 0;
			right_fork_held = 0;
			state = 's';
			time_sleep_start = cur_tp;
			printf("%lld, %d finished eating -> going to bed\n", (cur_tp - tab->start_tp), phi_n);
		}

		// sleep -> thinking if time_to_sleep has elapsed
		if (state == 's' && time_sleep_start + tab->time_to_sleep < cur_tp)
		{
			state = 't';
			printf("%lld, %d woke up -> Thinker's pose on the toilet\n", (cur_tp - tab->start_tp), phi_n);
		}
		usleep(5000);
	}
	return (NULL);
}

int philo_one(t_tab tab)
{
	// t_tab tab;

	// INITIALIZE struct values
	// tab.number_of_philosophers = 3;
	// tab.time_to_die = 16000;
	// tab.time_to_eat = 3000;
	// tab.time_to_sleep = 8000; // 1000ms = 1s
	// tab.state = malloc(tab.number_of_philosophers + 1);
	// tab.state[tab.number_of_philosophers] = '\0';
	// tab.available_forks = tab.number_of_philosophers;
	
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
		tab.phis[n].forks = 0;
		tab.phis[n].state = 'i';
		tab.phis[n].n_times_eaten = 0;
	}

	// GET starting time for time stamp
	struct timeval tp;
	gettimeofday(&tp, 0);
	// printf("tp: %ld.%d\n", tp.tv_sec, tp.tv_usec);
	tab.start_tp = tp.tv_sec;
	tab.start_tp *= 1000;
	tab.start_tp += (tp.tv_usec / 1000);
	// printf("start_tp: %lld\n", tab.start_tp);

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
		usleep(5000); // can only create threads as quickly as the phi_f function processes them
	}
	pthread_join(*phi_t, NULL);
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