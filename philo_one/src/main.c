#include "../philo_one.h"

void *return_error(t_tab *tab, int error_num)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX_LOCK)
		write(2, "pthread_mutex_lock() returned -1\n", 34);
	if (error_num == ERROR_MUTEX_UNLOCK)
		write(2, "pthread_mutex_unlock() returned -1\n", 36);
	if (error_num == ERROR_MUTEX_INIT)
		write(2, "pthread_mutex_init() didn't return 0\n", 38);
	if (error_num == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	// free_malloced_variables(tab);
	return (NULL);
}

void *phi_f(void *arg)
{
	t_tab *tab;

	int phi_n = -1; // sentinel value for an uninitialized philosopher
	int left_fork_held;
	int right_fork_held;
	int phi_state;
	long long time_last_meal;
	long long time_sleep_start;
	struct timeval tp;
	long long cur_tp;

	tab = (t_tab *)arg;
	// LIFE of a philosipher
	while(1)
	{
		// GET current time for time stamp
		cur_tp = get_current_time(tab);

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
			printf("%lld %d died\n", (cur_tp - tab->start_tp), phi_n + 1);
			tab->phi_died = 1;
			if (right_fork_held)
			{
				if (pthread_mutex_lock(&tab->forks[phi_n].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_LOCK));
				tab->forks[phi_n].available = 1;
				if (pthread_mutex_unlock(&tab->forks[phi_n].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_UNLOCK));
				right_fork_held = 0;
			}
			if (left_fork_held)
			{
				if (phi_n == 0)
				{
					if (pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
						return (return_error(tab, ERROR_MUTEX_LOCK));
					tab->forks[tab->number_of_philosophers - 1].available = 1;
					if (pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
						return (return_error(tab, ERROR_MUTEX_UNLOCK));
				}
				else
				{
					if (pthread_mutex_lock(&tab->forks[phi_n - 1].lock) == -1)
						return (return_error(tab, ERROR_MUTEX_LOCK));
					tab->forks[phi_n - 1].available = 1;
					if (pthread_mutex_unlock(&tab->forks[phi_n - 1].lock) == -1)
						return (return_error(tab, ERROR_MUTEX_UNLOCK));
				}
				left_fork_held = 0;
			}
			return (NULL);
		}

		// THINKING: grab forks if available 
		if (phi_state == 't')
		{
			if (pthread_mutex_lock(&tab->forks[phi_n].lock) == -1)
				return (return_error(tab, ERROR_MUTEX_LOCK));
			if (tab->forks[phi_n].available == 1)
			{
				tab->forks[phi_n].available = 0;
				right_fork_held = 1;
				printf("%lld %d has taken a fork (right)\n", (cur_tp - tab->start_tp), phi_n + 1);
			}
			if (pthread_mutex_unlock(&tab->forks[phi_n].lock) == -1)
				return (return_error(tab, ERROR_MUTEX_UNLOCK));
			if (phi_n == 0)
			{
				if (pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_LOCK));
				if (tab->forks[tab->number_of_philosophers - 1].available == 1)
				{
					tab->forks[tab->number_of_philosophers - 1].available = 0;
					left_fork_held = 1;
					printf("%lld %d has taken a fork (left)\n", (cur_tp - tab->start_tp), phi_n + 1);
				}
				if (pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_UNLOCK));
			}
			else
			{
				if (pthread_mutex_lock(&tab->forks[phi_n - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_LOCK));
				if (tab->forks[phi_n - 1].available == 1)
				{
					tab->forks[phi_n - 1].available = 0;
					left_fork_held++;
					printf("%lld %d has taken a fork (left)\n", (cur_tp - tab->start_tp), phi_n + 1);
				}
				if (pthread_mutex_unlock(&tab->forks[phi_n - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_UNLOCK));
			}
			// EATING, if 2 forks are in the philosophers posession: 
			if (left_fork_held && right_fork_held)
			{
				phi_state = 'e';
				time_last_meal = cur_tp;
				printf("%lld %d is eating\n", (cur_tp - tab->start_tp), phi_n + 1);
			}
		}

		// EATING -> sleeping, if time_to_eat has elapsed
		if (phi_state == 'e' && time_last_meal + tab->time_to_eat <= cur_tp)
		{
			if (pthread_mutex_lock(&tab->forks[phi_n].lock) == -1)
				return (return_error(tab, ERROR_MUTEX_LOCK));
			tab->forks[phi_n].available = 1;
			if (pthread_mutex_unlock(&tab->forks[phi_n].lock) == -1)
				return (return_error(tab, ERROR_MUTEX_UNLOCK));
			right_fork_held = 0;
			if (phi_n == 0)
			{
				if (pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_LOCK));
				tab->forks[tab->number_of_philosophers - 1].available = 1;
				if (pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_UNLOCK));
			}
			else
			{
				if (pthread_mutex_lock(&tab->forks[phi_n - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_LOCK));
				tab->forks[phi_n - 1].available = 1;
				if (pthread_mutex_unlock(&tab->forks[phi_n - 1].lock) == -1)
					return (return_error(tab, ERROR_MUTEX_UNLOCK));
			}
			left_fork_held = 0;
			// check if philo has eaten enough times yet
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
			printf("%lld %d is sleeping\n", (cur_tp - tab->start_tp), phi_n + 1);
		}

		// SLEEPING -> thinking, if time_to_sleep has elapsed
		if (phi_state == 's' && time_sleep_start + tab->time_to_sleep < cur_tp)
		{
			phi_state = 't';
			printf("%lld %d is thinking (on the toilet)\n", (cur_tp - tab->start_tp), phi_n + 1);
		}
		usleep(5000);
	}
	return (NULL);
}

int main(int ac, char **av)
{
	t_tab tab;

	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));

	// CREATE struct and initialize variables
	tab.number_of_philosophers = ft_atoi(av[1]);
	tab.time_to_die = ft_atoi(av[2]);
	tab.time_to_eat = ft_atoi(av[3]);
	tab.time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		tab.number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		tab.number_of_times_each_philosopher_must_eat = -1; // sentinel value for absence of value
	printf("number_of_philosophers: %d\n", tab.number_of_philosophers);
	printf("time_to_die: %d milliseconds\n", tab.time_to_die);
	printf("time_to_eat: %d milliseconds\n", tab.time_to_eat);
	printf("time_to_sleep: %d milliseconds\n", tab.time_to_sleep);
	printf("number_of_times_each_philosopher_must_eat: %d\n", tab.number_of_times_each_philosopher_must_eat);
	tab.phi_died = 0;
	if (!(tab.start_tp = get_current_time(&tab)))
		return (0);

	// CREATE fork struct & initialize locks
	if (!(tab.forks = malloc(sizeof(t_frk) * tab.number_of_philosophers)))
		return ((int)return_error(&tab, ERROR_MALLOC));
	int i = -1;
	while (++i < tab.number_of_philosophers)
	{
		if (pthread_mutex_init(&tab.forks[i].lock, NULL) != 0)
			return ((int)return_error(&tab, ERROR_MUTEX_INIT));
		tab.forks[i].available = 1;
	}
	if (!(tab.n_times_eaten = malloc(sizeof(int) * tab.number_of_philosophers)))
		return ((int)return_error(&tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab.number_of_philosophers)
		tab.n_times_eaten[i] = 0;

	// CREATE threads / start philosophers
	pthread_t *phi_t;
	if (!(phi_t = malloc(sizeof(pthread_t) * tab.number_of_philosophers + 1)))
		return ((int)return_error(&tab, ERROR_MALLOC));
	phi_t[tab.number_of_philosophers] = NULL;
	i = -1;
	while (++i < tab.number_of_philosophers)
	{
		tab.phi_n = i;
		if (pthread_create(&phi_t[i], NULL, phi_f, &tab) != 0)
			return ((int)return_error(&tab, ERROR_PTHREAD_CREATE));
		if (usleep(5000) == -1)
			return ((int)return_error(&tab, ERROR_USLEEP)); // tab.phi_n needs to be copied over in each phi_f thread, so we can only create threads as quickly as phi_f can copy
	}

	int returned = 0;
	while (returned < tab.number_of_philosophers)
	{
		if (usleep(100) == -1)
			return ((int)return_error(&tab, ERROR_USLEEP));
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