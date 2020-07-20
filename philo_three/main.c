#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()
#include "philosophers.h"
#include <semaphore.h> 

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	sem_t *forks_sem;
	int phi_died;
	int *n_times_eaten;
	int error_encountered;
} t_tab;

char	*ft_itoa(int n)
{
	int		neg;
	char	*str;
	int		len;
	long	n_cpy;

	n_cpy = n;
	neg = (n < 0) ? (1) : (0);
	n_cpy = (n_cpy < 0) ? (-n_cpy) : (n_cpy);
	len = 0;
	(n <= 0) && (len++);
	while (n)
		n = len++ ? n / 10 : n / 10;
	if (!(str = malloc(sizeof(char) * (len + 1))))
		return (NULL);
	str[len--] = '\0';
	(!n_cpy) && (str[len--] = '0');
	while (n_cpy)
	{
		str[len--] = (n_cpy % 10) + '0';
		n_cpy /= 10;
	}
	(neg == 1) && (str[len--] = '-');
	return (str);
}

void ft_putstring(char *s)
{
	while (*s)
		write(1, s++, 1);
}

int put_timestamp(long long time, int phi_n, char *message)
{
	char *c_time = ft_itoa((int)time);
	char *c_phi_n = ft_itoa(phi_n);
	ft_putstring(c_time);
	write(1, " ", 1);
	ft_putstring(c_phi_n);
	ft_putstring(message);
	write(1, "\n", 2);
}

void *return_error(t_tab *tab, int error_num)
{
	tab->error_encountered = 1;
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == 1)
		write(2, "pthread_mutex_(un)lock() returned -1\n", 38);
	if (error_num == 2)
		write(2, "gettimeofday() returned -1\n", 38);

	return (NULL);
}

void *phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;

	int phi_n = -1; // sentinel value for an uninitialized philosopher
	int forks_held;
	int phi_state;
	long long time_last_meal;
	long long time_sleep_start;
	struct timeval tp;
	long long cur_tp;

	sem_t *forks_sem = sem_open("forks", O_CREAT, 0644, 1);
	sleep(1);

	// LIFE of a philosipher
	while(1)
	{
		// GET current time for time stamp
		if (gettimeofday(&tp, 0) == -1)
			return (return_error(tab, 2));
		cur_tp = tp.tv_sec;
		cur_tp *= 1000;
		cur_tp += (tp.tv_usec / 1000);
		
		// INITIALIZE philosopher parameters
		if (phi_n == -1)
		{
			phi_n = tab->phi_n;
			forks_held = 0;
			phi_state = 's'; // 's' = sleep, 't' = thinking, 'e' = eating
			time_sleep_start = cur_tp;
			time_last_meal = cur_tp; // let's be nice and assume the philo's start on a full stomach
		}

// TEST -----------------------------------------------------------------------
		// if (phi_n == 1)
		// 	tab->phis[1].n_times_eaten = 2;
			// printf("fork availability: %d\n", forks_sem;
// ----------------------------------------------------------------------------

		// DETERMINE activity
		// DEATH, if time_to_die has elapsed
		if (time_last_meal + tab->time_to_die <= cur_tp)
		{
			// printf("%lld %d died\n", (cur_tp - tab->start_tp), phi_n + 1);
			put_timestamp((cur_tp - tab->start_tp), phi_n + 1, "died");
			tab->phi_died = 1;
			if (forks_held)
			{
				sem_wait(forks_sem);
				forks_sem = (sem_t *)((long long)forks_sem + forks_held);
				sem_post(forks_sem);
				forks_held = 0;
			}
			return (NULL);
		}

		// THINKING: grab forks if available 
		if (phi_state == 't')
		{
			sem_wait(forks_sem);
			if (forks_sem)
			{
				forks_sem = (sem_t *)((long long)forks_sem - 1);
				forks_held++;
				// printf("%lld %d has taken a fork\n", (cur_tp - tab->start_tp), phi_n + 1);
				put_timestamp((cur_tp - tab->start_tp), phi_n + 1, " has taken a fork");
			}
			if (forks_held != 2 && forks_sem)
			{
				forks_sem = (sem_t *)((long long)forks_sem - 1);
				forks_held++;
				put_timestamp((cur_tp - tab->start_tp), phi_n + 1, " has taken a fork");
			}
			sem_post(forks_sem);
			if (forks_held == 2)
			{
				phi_state = 'e';
				time_last_meal = cur_tp;
				put_timestamp((cur_tp - tab->start_tp), phi_n + 1, " is eating");
				// printf("%lld %d is eating\n", (cur_tp - tab->start_tp), phi_n + 1);
				// TESTING ----------------------------------------------------
				// printf("forks available: %d\n", (int)forks_sem);
				// ------------------------------------------------------------
			}
		}

		// EATING -> sleeping, if time_to_eat has elapsed
		if (phi_state == 'e' && time_last_meal + tab->time_to_eat <= cur_tp)
		{
			sem_wait(forks_sem);
			forks_sem = (sem_t *)((long long)forks_sem + 2);
			sem_post(forks_sem);
			forks_held = 0;
			// CHECK if philo has eaten enough times yet
			tab->n_times_eaten[phi_n]++;
			if (tab->number_of_times_each_philosopher_must_eat != -1
			&& tab->n_times_eaten[phi_n]
			>= tab->number_of_times_each_philosopher_must_eat)
			{
				// TESTING ----------------------------------------------------
				printf("%lld, %d has fattened up enough\n", (cur_tp - tab->start_tp), phi_n + 1);
				// ------------------------------------------------------------
				return (NULL);
			}
			phi_state = 's';
			time_sleep_start = cur_tp;
			put_timestamp((cur_tp - tab->start_tp), phi_n + 1, " is sleeping");
			// printf("%lld %d is sleeping\n", (cur_tp - tab->start_tp), phi_n + 1);
		}

		// SLEEPING -> thinking, if time_to_sleep has elapsed
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < cur_tp)
		{
			phi_state = 't';
			put_timestamp((cur_tp - tab->start_tp), phi_n + 1, " is thinking");
			// printf("%lld %d is thinking (on the toilet)\n", (cur_tp - tab->start_tp), phi_n + 1);
		}
		usleep(5000);
	}
	return (NULL);
}

int main(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		write(2, B_RED"error: too few or too many arguments\n"RESET, 49);
		return (0);
	}

	// DECLARE struct and initialize variables
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
	tab.phi_died = 0;

	// CREATE semaphore
	// tab.forks_sem = sem_open("forks", O_CREAT, 0644, tab.number_of_philosophers);
	// sem_close(tab.forks_sem);

	tab.n_times_eaten = malloc(sizeof(int) * tab.number_of_philosophers);
	int i = -1;
	while (++i < tab.number_of_philosophers)
		tab.n_times_eaten[i] = 0;

	// GET starting time for time stamp
	struct timeval tp;
	if (gettimeofday(&tp, 0) == -1)
		return (0);
	tab.start_tp = tp.tv_sec;
	tab.start_tp *= 1000;
	tab.start_tp += (tp.tv_usec / 1000);

	// CREATE threads / start philosophers
	// pthread_t *phi_t;
	// phi_t = malloc(sizeof(pthread_t) * tab.number_of_philosophers + 1);
	// phi_t[tab.number_of_philosophers] = NULL;
	// i = -1;
	// while (++i < tab.number_of_philosophers)
	// {
	// 	tab.phi_n = i;
	// 	pthread_create(&phi_t[i], NULL, phi_f, &tab);
	// 	usleep(5000); // tab.phi_n needs to be copied over in each phi_f thread, so we can only create threads as quickly as phi_f can copy
	// }

	// CREATE child processes / start philosophers
	i = -1;
	while (++i < tab.number_of_philosophers)
	{
		tab.phi_n = i;
		pid_t fork_ret = fork();
		if (fork_ret == 0)
		{
			phi_f(&tab);
		}
		usleep(5000);
	}

	sem_unlink("forks");

	// CHECK if one has died or all have returned
	int returned = 0;
	while (returned < tab.number_of_philosophers)
	{
		usleep(100);
		if (tab.phi_died)
		{
			write(1, B_RED"A philosopher has starved! Game over.\n\033[0m"RESET, 50);
			return (0);
		}
		if (tab.error_encountered)
			return (0);
		i = -1;
		returned = 0;
		while (++i < tab.number_of_philosophers)
		{
			if (tab.n_times_eaten[i] == tab.number_of_times_each_philosopher_must_eat)
				returned++;
			if (returned == tab.number_of_philosophers)
			{
				write(1, B_GREEN"They're all fat. Welcome to America!\n"RESET, 49);
				return (0);
			}
		}
	}
	return (0);
}