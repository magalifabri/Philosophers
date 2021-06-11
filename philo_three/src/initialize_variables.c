#include "../philo_three.h"

/*
"sem_unlink("fork_sem");" is required in case the program was
exited badly the previous time and the semaphore wasn't unlinked. Possible
errors are irrelevant enough to ignore them.

"tab->phi_pid[i] = -1;" sentinel value.
*/

int	initialize_variables_part_2(t_tab *tab)
{
	int	i;

	sem_unlink("fork_sem");
	tab->fork_sem = sem_open("fork_sem", O_CREAT, 0644,
			tab->number_of_philosophers / 2);
	if (tab->fork_sem == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	tab->fork_sem_initialised = 1;
	sem_unlink("print_sem");
	tab->print_sem = sem_open("print_sem", O_CREAT, 0644, 1);
	if (tab->print_sem == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	tab->print_sem_initialised = 1;
	tab->times_eaten = 0;
	tab->phi_pid = malloc(sizeof(int) * tab->number_of_philosophers);
	if (!tab->phi_pid)
		return ((int)return_error(tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab->number_of_philosophers)
		tab->phi_pid[i] = -1;
	tab->start_time = get_current_time(tab);
	if (tab->start_time == -1)
		return (0);
	return (1);
}

/*
"tab->number_of_times_each_philosopher_must_eat = -1" is a sentinel value,
as an indication that no input for the variable was given.
*/

int	initialize_variables(t_tab *tab, int ac, char **av)
{
	if (ac < 5 || ac > 6)
		return ((int)return_error(tab, ERROR_AC));
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->number_of_philosophers > 300
		|| tab->time_to_die < 1
		|| tab->time_to_eat < 1
		|| tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (tab->number_of_times_each_philosopher_must_eat < 1)
			return ((int)return_error(tab, ERROR_BAD_ARGS));
	}
	return (initialize_variables_part_2(tab));
}

void	pre_initialisation(t_tab *tab)
{
	tab->phi_pid = NULL;
	tab->fork_sem_initialised = 0;
	tab->print_sem_initialised = 0;
}
