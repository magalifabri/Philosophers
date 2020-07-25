#include "../philo_one.h"

	// TESTING ----------------------------------------------------------------
	// printf("number_of_philosophers: %d\n", tab->number_of_philosophers);
	// printf("time_to_die: %d milliseconds\n", tab->time_to_die);
	// printf("time_to_eat: %d milliseconds\n", tab->time_to_eat);
	// printf("time_to_sleep: %d milliseconds\n", tab->time_to_sleep);
	// printf("number_of_times_each_philosopher_must_eat: %d\n", tab->number_of_times_each_philosopher_must_eat);
	// ------------------------------------------------------------------------


void initialize_malloc_indicators(t_tab *tab)
{
	tab->malloc_forks = 0;
	tab->malloc_n_times_eaten = 0;
	tab->malloc_phi_t = 0;
}

static int initialize_more(t_tab *tab)
{
	int i;

	if (!(tab->start_time = get_current_time(tab)))
		return (0);
	if (!(tab->forks = malloc(sizeof(t_frk) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->malloc_forks = 1;
	if (!(tab->n_times_eaten
	= malloc(sizeof(int) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->malloc_n_times_eaten = 1;
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (pthread_mutex_init(&tab->forks[i].lock, NULL) != 0)
			return ((int)return_error(tab, ERROR_MUTEX_INIT));
		tab->forks[i].available = 1;
	}
	return (1);
}

/*
"tab->number_of_times_each_philosopher_must_eat = -1;": sentinel value for absence of value
*/

int initialize_variables_and_locks(t_tab *tab, int ac, char **av)
{
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->time_to_die < 1
	|| tab->time_to_eat < 1 || tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else if ((tab->number_of_times_each_philosopher_must_eat
	= ft_atoi(av[5])) == 0)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	tab->phi_died = 0;
	tab->error_encountered = 0;
	if (!initialize_more(tab))
		return (0);
	return (1);
}

void initialize_variables_phi_f(t_tab *tab, t_thread_var_struct *s)
{
	s->phi_n = tab->phi_n;
	s->left_fork_held = 0;
	s->right_fork_held = 0;
	s->phi_state = 't';
	s->time_sleep_start = tab->current_time;
	s->time_last_meal = tab->current_time;
}