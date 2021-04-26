#include "../philo_one.h"

int	exit_error(t_tab *tab)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (tab->error_code == ERROR_MUTEX_LOCK)
		write(2, "pthread_mutex_lock() returned -1\n", 34);
	else if (tab->error_code == ERROR_MUTEX_UNLOCK)
		write(2, "pthread_mutex_unlock() returned -1\n", 36);
	else if (tab->error_code == ERROR_MUTEX_INIT)
		write(2, "pthread_mutex_init() didn't return 0\n", 38);
	else if (tab->error_code == ERROR_MUTEX_DESTROY)
		write(2, "pthread_mutex_destroy() didn't return 0\n", 41);
	else if (tab->error_code == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (tab->error_code == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (tab->error_code == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (tab->error_code == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (tab->error_code == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	if (tab->mutexes_initialized)
		destroy_locks(tab);
	free_malloced_variables(tab);
	return (1);
}

void	*set_error_code(t_tab *tab, int error_code)
{
	tab->error_code = error_code;
	return (NULL);
}

int	check_if_all_are_sated(t_tab *tab)
{
	int	i;
	int	number_of_fat_philosophers;

	i = -1;
	number_of_fat_philosophers = 0;
	while (++i < tab->number_of_philosophers)
	{
		if (tab->n_times_eaten[i]
			== tab->number_of_times_each_philosopher_must_eat)
			number_of_fat_philosophers++;
		if (number_of_fat_philosophers == tab->number_of_philosophers)
		{
			printf(B_GREEN"They're all fat. Good job!\n"RESET);
			return (1);
		}
	}
	return (0);
}

/*
** Note(s) on monitor_philosophers():
** 
** Instead of using pthread_join() to make sure the main process doesn't exit
** before the threads are done, we trap the main process in a loop that it will
** only exit when the threads are done (a philosopher dies, all are fat or an
** error occurs).
** 
** This function supplies the value to the tab.current_time variable that is
** used by the threads. This is done so that they don't each have to do this
** individually.
*/

int	monitor_philosophers(t_tab *tab)
{
	while (1)
	{
		if (usleep(1000) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
		tab->current_time = get_current_time(tab);
		if (!tab->current_time)
			return (0);
		if (tab->error_code)
			return (0);
		if (tab->phi_died)
		{
			printf(B_RED"A philosopher has starved! Game over.\n"RESET);
			return (1);
		}
		if (check_if_all_are_sated(tab))
			return (1);
	}
	return (0);
}

/*
** Note(s) on create_philosophers():
** 
** Reason for usleep(): tab.phi_n needs to be copied over in each phi_f thread
** to tell the thread the number of the philosopher it represents. So we want
** to give each thread a bit of time to copy this value.
*/

int	create_philosophers(t_tab *tab)
{
	int	i;

	i = -1;
	while (++i < tab->number_of_philosophers
	&& !tab->phi_died && !tab->error_code)
	{
		tab->current_time = get_current_time(tab);
		if (!tab->current_time)
			return (0);
		tab->phi_n = i;
		if (pthread_create(&tab->phi_t[i], NULL, phi_f, tab) != 0)
			return ((int)set_error_code(tab, ERROR_PTHREAD_CREATE));
		pthread_detach(tab->phi_t[i]);
		if (usleep(100) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
	}
	return (1);
}

int	main(int ac, char **av)
{
	t_tab	tab;

	if (!initialize_variables_and_locks(&tab, ac, av))
		return (exit_error(&tab));
	if (!create_philosophers(&tab))
		return (exit_error(&tab));
	if (!monitor_philosophers(&tab))
		return (exit_error(&tab));
	if (!destroy_locks(&tab))
		return (exit_error(&tab));
	free_malloced_variables(&tab);
	return (0);
}
