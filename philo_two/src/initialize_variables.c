#include "../philo_two.h"

/*
** Note(s) on initialize_variables_part_2():
** 
** "sem_unlink("fork_availability");" is required in case the program was
** exited badly the previous time and the semaphore wasn't unlinked. Possible
** errors are irrelevant enough to ignore them.
*/

static int	initialize_more(t_tab *tab)
{
	int	i;

	tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers);
	tab->phi_t = malloc(sizeof(pthread_t) * tab->number_of_philosophers);
	if (!tab->n_times_eaten || !tab->phi_t)
		return ((int)return_error(tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab->number_of_philosophers)
		tab->n_times_eaten[i] = 0;
	i = -1;
	sem_unlink("fork_availability");
	tab->fork_availability = sem_open("fork_availability", O_CREAT, 0644,
			tab->number_of_philosophers / 2);
	if (tab->fork_availability == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	return (1);
}

/*
** Note(s) on initialize_variables_and_locks():
** 
** "tab->number_of_times_each_philosopher_must_eat = -1": -1 is a sentinel
** value for an absence of value (a value for it wasn't supplied with av)
*/

int	initialize_variables(t_tab *tab, int ac, char **av)
{
	tab->n_times_eaten = NULL;
	tab->phi_t = NULL;
	if (ac < 5 || ac > 6)
		return ((int)return_error(tab, ERROR_AC));
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->time_to_die < 1
		|| tab->time_to_eat < 1 || tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (tab->number_of_times_each_philosopher_must_eat < 1)
			return ((int)return_error(tab, ERROR_BAD_ARGS));
	}
	tab->phi_died = 0;
	tab->error_encountered = 0;
	tab->start_time = get_current_time(tab);
	if (!tab->start_time)
		return (0);
	return (initialize_more(tab));
}
