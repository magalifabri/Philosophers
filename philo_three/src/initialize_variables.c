#include "../philo_three.h"

int	initialize_philosophers(t_tab *tab)
{
	int		i;
	pid_t	fork_ret;

	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		tab->phi_n = i;
		fork_ret = fork();
		if (fork_ret == 0)
			phi_f(tab);
		else if (fork_ret > 0)
			tab->phi_pid[i] = fork_ret;
		else
			return ((int)return_error(tab, ERROR_FORK));
	}
	return (1);
}

/*
** Note(s) on initialize_variables_part_2():
** 
** "sem_unlink("fork_availability");" is required in case the program was
** exited badly the previous time and the semaphore wasn't unlinked. Possible
** errors are irrelevant enough to ignore them.
*/

int	initialize_variables_part_2(t_tab *tab)
{
	int	i;

	sem_unlink("fork_availability");
	tab->fork_availability = sem_open("fork_availability", O_CREAT, 0644,
			tab->number_of_philosophers / 2);
	if (tab->fork_availability == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	i = -1;
	tab->times_eaten = 0;
	tab->start_time = get_current_time(tab);
	if (tab->start_time == -1)
		return (0);
	tab->phi_pid = malloc(sizeof(int) * tab->number_of_philosophers);
	if (!tab->phi_pid)
		return ((int)return_error(tab, ERROR_MALLOC));
	return (1);
}

/*
** Note(s) on initialize_variables():
** 
** "tab->number_of_times_each_philosopher_must_eat = -1" is a sentinel value,
** as an indication that no input for the variable was given.
*/

int	initialize_variables(t_tab *tab, int ac, char **av)
{
	tab->phi_pid = NULL;
	if (ac < 5 || ac > 6)
		return ((int)return_error(tab, ERROR_AC));
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (tab->number_of_times_each_philosopher_must_eat < 1)
			return ((int)return_error(tab, ERROR_BAD_ARGS));
	}
	if (tab->number_of_philosophers < 2
		|| tab->time_to_die < 1
		|| tab->time_to_eat < 1
		|| tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	return (initialize_variables_part_2(tab));
}
