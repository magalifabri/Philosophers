#include "../philo_two.h"

/*
** Note(s) on initialize_variables_part_2():
** 
** "sem_unlink(x);" is required in case the program was
** exited badly the previous time and the semaphores weren't unlinked. Possible
** errors are irrelevant enough to ignore them.
*/

static int	initialize_more(t_tab *tab)
{
	int	i;

	tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers);
	if (!tab->n_times_eaten)
		return ((int)set_exit_code(tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab->number_of_philosophers)
		tab->n_times_eaten[i] = 0;
	i = -1;
	sem_unlink("fork_sem");
	sem_unlink("id_sem");
	sem_unlink("print_sem");
	tab->fork_sem = sem_open("fork_sem", O_CREAT, 0644,
			tab->number_of_philosophers / 2);
	tab->id_sem = sem_open("id_sem", O_CREAT, 0644, 1);
	tab->print_sem = sem_open("print_sem", O_CREAT, 0644, 1);
	if (tab->fork_sem == SEM_FAILED || tab->id_sem == SEM_FAILED
		|| tab->print_sem == SEM_FAILED)
		return ((int)set_exit_code(tab, ERROR_SEM_OPEN));
	tab->start_time = get_current_time();
	if (tab->start_time == -1)
		return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
	return (1);
}

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
	tab->number_of_fat_philosophers = 0;
	return (initialize_more(tab));
}
