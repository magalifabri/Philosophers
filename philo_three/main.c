#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> // required for gettimeofday()
#include "philosophers.h"
#include <semaphore.h> 
#include <signal.h> // required for kill()

#define EXIT_EATEN_ENOUGH 0
#define EXIT_DEATH 3
#define EXIT_ERROR 2

#define ERROR_MUTEX 1
#define ERROR_GETTIMEOFDAY 2
#define ERROR_MALLOC 3
#define ERROR_PTHREAD_CREATE 4
// #define ERROR_
// #define ERROR_


// typedef struct s_philosophers
// {
// 	// int phi_pid;
// 	// int time_last_eaten;
// } t_philosophers;

typedef struct s_tab
{
	long long start_tp;
	int phi_n;
	int number_of_philosophers;
	int time_to_die;   // time in ms before the next meal needs to start
	int time_to_eat;   // duration in ms that the philo will spend eating
	int time_to_sleep; // duration in ms that the philosopher will spend sleeping
	int number_of_times_each_philosopher_must_eat;
	sem_t *fork_availability;
	// t_philosophers *phis;
	// int forks_available;
	long long *time_last_meal;
	int phi_died;
	int *n_times_eaten;
	int *phi_pid;
	int error_encountered;
} t_tab;

size_t	ft_strlen_2(const char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strjoin_2(char const *s1, char const *s2)
{
	unsigned int	s1_len;
	unsigned int	s2_len;
	char			*s3;
	unsigned int	i;

	if (!s1 || !s2)
		return (NULL);
	s1_len = ft_strlen_2(s1);
	s2_len = ft_strlen_2(s2);
	if (!(s3 = (char *)malloc(sizeof(char) * (s1_len + s2_len + 1))))
		return (NULL);
	i = 0;
	while (i < s1_len)
		s3[i++] = *s1++;
	while (i < (s1_len + s2_len))
		s3[i++] = *s2++;
	s3[i] = '\0';
	return (s3);
}

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
	int len = ft_strlen_2(s);
	// while (*s)
		write(1, s, len);
}

int put_timestamp(long long time, int phi_n, char *message)
{
	char *c_time;
	char *c_phi_n;
	char *concat;

	c_time = ft_itoa((int)time);
	c_phi_n = ft_itoa(phi_n);
	c_time = ft_strjoin_2(c_time, " ");
	c_phi_n = ft_strjoin_2(c_phi_n, " ");
	concat = ft_strjoin_2(c_time, c_phi_n);
	concat = ft_strjoin_2(concat, message);
	// ft_putstring(c_time);
	// write(1, " ", 1);
	// ft_putstring(c_phi_n);
	ft_putstring(concat);
	// write(1, "\n", 2);
	return (0);
}

void *return_error(t_tab *tab, int error_num)
{
	tab->error_encountered = 1;
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX)
		write(2, "pthread_mutex_(un)lock() returned -1\n", 38);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 38);

	return (NULL);
}

long long get_passed_time(t_tab *tab)
{
	struct timeval tp;
	long long passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)return_error(tab, 2));
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}

void *death_signaller(void *arg)
{
	t_tab *tab;
	tab = (t_tab *)arg;

	while (1)
	{
		if (tab->time_last_meal[tab->phi_n] + tab->time_to_die <= get_passed_time(tab))
		{
			put_timestamp((get_passed_time(tab) - tab->start_tp), tab->phi_n + 1, "died\n");
			tab->phi_died = 1;
			exit(EXIT_DEATH);
		}
		usleep(5000);
	}
}

void phi_f(void *arg)
{
	t_tab *tab = (t_tab *)arg;

	int phi_n = -1; // sentinel value for an uninitialized philosopher
	int forks_held;
	int phi_state;
	long long time_last_meal;
	long long time_sleep_start;

	// STARVATION monitor
	pthread_t death_signaller_t;
	pthread_create(&death_signaller_t, NULL, death_signaller, tab);

	// LIFE of a philosipher
	while(1)
	{
		// // GET current time for time stamp
		// if (gettimeofday(&tp, 0) == -1)
		// 	return (return_error(tab, 2));
		// cur_tp = tp.tv_sec;
		// cur_tp *= 1000;
		// cur_tp += (tp.tv_usec / 1000);
		
		// INITIALIZE philosopher parameters
		if (phi_n == -1)
		{
			phi_n = tab->phi_n;
			// forks_held = 0;
			phi_state = 's'; // 's' = sleep, 't' = thinking, 'e' = eating
			time_sleep_start = get_passed_time(tab);
			tab->time_last_meal[phi_n] = get_passed_time(tab); // let's be nice and assume the philo's start on a full stomach
		}

// TEST -----------------------------------------------------------------------
		// if (phi_n == 1)
		// 	tab->phis[1].n_times_eaten = 2;
			// printf("fork availability: %d\n", tab->fork_availability;
// ----------------------------------------------------------------------------

		// DETERMINE activity
		// DEATH, if time_to_die has elapsed
		if (tab->time_last_meal[phi_n] + tab->time_to_die <= get_passed_time(tab))
		{
			// printf("%lld %d died\n", (cur_tp - tab->start_tp), phi_n + 1);
			put_timestamp((get_passed_time(tab) - tab->start_tp), phi_n + 1, "died\n");
			tab->phi_died = 1;
			exit(EXIT_DEATH);
		}

		// THINKING: eat when <= half of the philosophers are eating (then enough forks are available)
		if (phi_state == 't')
		{
			sem_wait(tab->fork_availability);
			if (tab->phi_died)
				exit(EXIT_DEATH);
			phi_state = 'e';
			tab->time_last_meal[phi_n] = get_passed_time(tab);
			put_timestamp((get_passed_time(tab) - tab->start_tp), phi_n + 1, "is eating\n");
			// sem_post(tab->fork_availability);
		}

		// EATING -> sleeping, if time_to_eat has elapsed
		if (phi_state == 'e' && tab->time_last_meal[phi_n] + tab->time_to_eat <= get_passed_time(tab))
		{
			// sem_post(tab->fork_availability);
			// CHECK if philo has eaten enough times yet
			tab->n_times_eaten[phi_n]++;
			if (tab->number_of_times_each_philosopher_must_eat != -1
			&& tab->n_times_eaten[phi_n]
			>= tab->number_of_times_each_philosopher_must_eat)
			{
				// TESTING ----------------------------------------------------
				printf("%lld, %d has fattened up enough\n", (get_passed_time(tab) - tab->start_tp), phi_n + 1);
				// ------------------------------------------------------------
				sem_post(tab->fork_availability);
				exit(EXIT_EATEN_ENOUGH);
			}
			phi_state = 's';
			time_sleep_start = get_passed_time(tab);
			put_timestamp((get_passed_time(tab) - tab->start_tp), phi_n + 1, "is sleeping\n");
			sem_post(tab->fork_availability);
		}

		// SLEEPING -> thinking, if time_to_sleep has elapsed
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < get_passed_time(tab))
		{
			phi_state = 't';
			put_timestamp((get_passed_time(tab) - tab->start_tp), phi_n + 1, "is thinking\n");
			// printf("%lld %d is thinking (on the toilet)\n", (cur_tp - tab->start_tp), phi_n + 1);
		}
		usleep(5000);
	}
	// exit(0);
}

int main(int ac, char **av)
{
	// CHECK number of arguments
	if (ac < 5 || ac > 6)
	{
		write(2, B_RED"ERROR: "RESET"too few or too many arguments\n", 49);
		return (0);
	}

	// DECLARE struct and initialize variables
	t_tab tab;
	tab.number_of_philosophers = atoi(av[1]);
	write(1, UNDERLINE"Program Configurations:\n"RESET, 33);
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
	sem_unlink("fork_availability");
	tab.fork_availability = sem_open("fork_availability", O_CREAT, 0644, tab.number_of_philosophers / 2);

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

	// MALLOC space for t_philosopher struct variables
	tab.time_last_meal = malloc(sizeof(long long) * tab.number_of_philosophers);
	tab.phi_pid = malloc(sizeof(int) * tab.number_of_philosophers);
	
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
		else if (fork_ret > 0)
		{
			// TESTING --------------------------------------------------------
			printf("pid of child %d is %d\n", i, (int)fork_ret);
			// ----------------------------------------------------------------
			tab.phi_pid[i] = fork_ret;
		}
		usleep(5000);
	}

	// WAIT for a process/philosopher to die or all to fatten up
	int philosophers_left = tab.number_of_philosophers;
	int exit_status;
	pid_t pid;
	while (philosophers_left--)
	{
		pid = wait(&exit_status);
		printf("philosopher with id %d exited with status %d\n", (int)pid, WEXITSTATUS(exit_status));
		// while
		if (WEXITSTATUS(exit_status) == EXIT_DEATH)
		{
			while (--tab.number_of_philosophers > -1)
			{
				printf("kill'ing philosopher with pid %d\n", tab.phi_pid[tab.number_of_philosophers]);
				kill(tab.phi_pid[tab.number_of_philosophers], SIGKILL);

			}
			printf("all philosophers kill'ed\n");
			break;
		}
		// As soon as wait() returns with a pid, we know a philospopher has exited, either because he's fat or because he's dead. If he's fat, we let the other philosopher's continue; if he's dead, we kill the other philosophers. In order to kill the other philosophers we have to 
	}
	printf("all philosophers are done\n");

	// sem_unlink("fork_availability");

	// // CHECK if one has died or all have returned
	// int returned = 0;
	// while (returned < tab.number_of_philosophers)
	// {
	// 	usleep(100);
	// 	if (tab.phi_died)
	// 	{
	// 		write(1, B_RED"A philosopher has starved! Game over.\n\033[0m"RESET, 50);
	// 		return (0);
	// 	}
	// 	if (tab.error_encountered)
	// 		return (0);
	// 	i = -1;
	// 	returned = 0;
	// 	while (++i < tab.number_of_philosophers)
	// 	{
	// 		if (tab.n_times_eaten[i] == tab.number_of_times_each_philosopher_must_eat)
	// 			returned++;
	// 		if (returned == tab.number_of_philosophers)
	// 		{
	// 			write(1, B_GREEN"They're all fat. Welcome to America!\n"RESET, 49);
	// 			return (0);
	// 		}
	// 	}
	// }
	return (0);
}