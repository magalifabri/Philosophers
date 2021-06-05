#include "../philo_one.h"

void	pre_initialisation(t_tab *tab)
{
	tab->exit_code = 0;
	tab->forks = NULL;
	tab->n_times_eaten = NULL;
	tab->n_fork_locks_initialized = 0;
	tab->print_lock_initialized = 0;
	tab->id_lock_initialized = 0;
	tab->pthreads_created = 0;
}

static int	initialize_more(t_tab *tab)
{
	int	i;

	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		tab->n_times_eaten[i] = 0;
		tab->time_last_meal[i] = 0;
	}
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (pthread_mutex_init(&tab->forks[i].lock, NULL) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_INIT));
		tab->forks[i].available = 1;
		tab->n_fork_locks_initialized++;
	}
	if (pthread_mutex_init(&tab->print_lock, NULL) != 0)
		return ((int)set_exit_code(tab, ERROR_MUTEX_INIT));
	tab->print_lock_initialized = 1;
	if (pthread_mutex_init(&tab->id_lock, NULL) != 0)
		return ((int)set_exit_code(tab, ERROR_MUTEX_INIT));
	tab->id_lock_initialized = 1;
	tab->start_time = get_current_time();
	if (tab->start_time == -1)
		return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
	return (1);
}

/*
Note(s) on initialize_variables():

"tab->number_of_times_each_philosopher_must_eat = -1": -1 is a sentinel
value for an absence of value (a value for it wasn't supplied with av)
*/

int	initialize_variables(t_tab *tab, int ac, char **av)
{
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->time_to_die < 1
		|| tab->time_to_eat < 1 || tab->time_to_sleep < 1)
		return ((int)set_exit_code(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (tab->number_of_times_each_philosopher_must_eat < 1)
			return ((int)set_exit_code(tab, ERROR_BAD_ARGS));
	}
	tab->forks = malloc(sizeof(t_frk) * tab->number_of_philosophers);
	tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers);
	tab->time_last_meal
		= malloc(sizeof(long long) * tab->number_of_philosophers);
	if (!tab->forks || !tab->n_times_eaten || !tab->time_last_meal)
		return ((int)set_exit_code(tab, ERROR_MALLOC));
	tab->number_of_fat_philosophers = 0;
	return (initialize_more(tab));
}
