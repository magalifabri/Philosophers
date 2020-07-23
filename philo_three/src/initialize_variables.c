#include "../philo_three.h"

/*
ABOUT initialize_variables_part_2()

"sem_unlink("fork_availability");" is required in case the program was exited badly the previous time and the semaphore wasn't unlinked. Possible errors are irrelevant enough to ignore them.
*/

int initialize_variables_part_2(t_tab *tab)
{
	int i;

	sem_unlink("fork_availability");
	if ((tab->fork_availability = sem_open("fork_availability", O_CREAT, 0644
	, tab->number_of_philosophers / 2)) == SEM_FAILED)
		return ((int)return_error(tab, ERROR_SEM_OPEN));
	i = -1;
	tab->times_eaten = 0;
	if ((tab->start_time = get_current_time(tab)) == -1)
		return (0);
	if (!(tab->phi_pid = malloc(sizeof(int) * tab->number_of_philosophers)))
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->malloc_phi_pid = 1;
	return (1);
}

/*
ABOUT initialize_variables()

"tab->number_of_times_each_philosopher_must_eat = -1" is a sentinel value, as an indication that no input for the variable was given.
*/

int initialize_variables(t_tab *tab, int ac, char **av)
{
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else if ((tab->number_of_times_each_philosopher_must_eat
	= ft_atoi(av[5])) == 0)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	if (tab->number_of_philosophers < 2
	|| tab->time_to_die < 1
	|| tab->time_to_eat < 1
	|| tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));

	// TESTING ----------------------------------------------------------------
	write(1, UNDERLINE"Program Configurations:\n"RESET, 33);
	printf("number_of_philosophers: %d\n", tab->number_of_philosophers);
	printf("time_to_die: %d milliseconds\n", tab->time_to_die);
	printf("time_to_eat: %d milliseconds\n", tab->time_to_eat);
	printf("time_to_sleep: %d milliseconds\n", tab->time_to_sleep);
	printf("number_of_times_each_philosopher_must_eat: %d\n", tab->number_of_times_each_philosopher_must_eat);
	// ------------------------------------------------------------------------

	initialize_variables_part_2(tab);
	return (1);
}
