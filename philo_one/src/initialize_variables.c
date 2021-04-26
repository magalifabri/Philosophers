#include "../philo_one.h"

void	initialize_malloc_and_mutex_indicators(t_tab *tab)
{
	tab->forks = NULL;
	tab->n_times_eaten = NULL;
	tab->mutexes_initialized = 0;
}

static int	initialize_more(t_tab *tab)
{
	int	i;

	tab->forks = malloc(sizeof(t_frk) * tab->number_of_philosophers);
	tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers);
	if (!tab->forks || !tab->n_times_eaten)
		return ((int)set_error_code(tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab->number_of_philosophers)
		tab->n_times_eaten[i] = 0;
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (pthread_mutex_init(&tab->forks[i].lock, NULL) != 0)
			return ((int)set_error_code(tab, ERROR_MUTEX_INIT));
		tab->forks[i].available = 1;
	}
	if (pthread_mutex_init(&tab->put_status_lock, NULL) != 0)
		return ((int)set_error_code(tab, ERROR_MUTEX_INIT));
	// if (pthread_mutex_init(&tab->id_lock, NULL) != 0)
	// 	return ((int)set_error_code(tab, ERROR_MUTEX_INIT));
	if (pthread_mutex_init(&tab->death_lock, NULL) != 0)
		return ((int)set_error_code(tab, ERROR_MUTEX_INIT));
	tab->mutexes_initialized = 1;
	return (1);
}

/*
** Note(s) on initialize_variables_and_locks():
** 
** "tab->number_of_times_each_philosopher_must_eat = -1": -1 is a sentinel
** value for an absence of value (a value for it wasn't supplied with av)
*/

int	initialize_variables_and_locks(t_tab *tab, int ac, char **av)
{
	initialize_malloc_and_mutex_indicators(tab);
	if (ac < 5 || ac > 6)
		return ((int)set_error_code(tab, ERROR_AC));
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->time_to_die < 1
		|| tab->time_to_eat < 1 || tab->time_to_sleep < 1)
		return ((int)set_error_code(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (tab->number_of_times_each_philosopher_must_eat < 1)
			return ((int)set_error_code(tab, ERROR_BAD_ARGS));
	}
	tab->phi_died = 0;
	tab->number_of_fat_philosophers = 0;
	tab->all_fat = 0;
	tab->error_code = 0;
	tab->start_time = get_current_time(tab);
	if (!tab->start_time)
		return (0);
	return (initialize_more(tab));
}

int	initialize_variables_phi_f(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->id_lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_LOCK));
	s->phi_n = tab->phi_n_c++;
	if (pthread_mutex_unlock(&tab->id_lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_UNLOCK));
	s->got_forks = 0;
	s->phi_state = 't';
	s->time_sleep_start = tab->current_time;
	s->time_last_meal = tab->current_time;
	return (1);
}
