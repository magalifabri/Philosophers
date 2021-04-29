#include "../philo_two.h"

static int	exit_error(t_tab *tab)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (tab->exit_code == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (tab->exit_code == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (tab->exit_code == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (tab->exit_code == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (tab->exit_code == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (tab->exit_code == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (tab->exit_code == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (tab->exit_code == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (tab->exit_code == ERROR_SEM_WAIT)
		write(2, "sem_wait() returned -1\n", 24);
	else if (tab->exit_code == ERROR_SEM_POST)
		write(2, "sem_post() returned -1\n", 24);
	wrap_up(tab);
	return (1);
}

long long	get_current_time(void)
{
	struct timeval	tp;
	long long		passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return (-1);
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
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

static int	monitor_philosophers(t_tab *tab)
{
	while (1)
	{
		if (usleep(1000) == -1)
			return ((int)set_exit_code(tab, ERROR_USLEEP));
		tab->current_time = get_current_time();
		if (tab->current_time == -1)
			return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
		if (tab->exit_code == DEATH)
		{
			printf(B_RED"A philosopher has starved! Game over."RESET"\n");
			return (1);
		}
		else if (tab->exit_code == ALL_FAT)
		{
			printf(B_GREEN"They're all fat. Good job!\n"RESET);
			return (1);
		}
		else if (tab->exit_code)
			return (0);
	}
	return (0);
}

static int	create_philosophers(t_tab *tab)
{
	int			i;

	tab->phi_n_c = 0;
	i = -1;
	tab->current_time = get_current_time();
	if (tab->current_time == -1)
		return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
	while (++i < tab->number_of_philosophers && !tab->exit_code)
		if (pthread_create(&tab->philosopher_thread, NULL, phi_f, tab) != 0)
			return ((int)set_exit_code(tab, ERROR_PTHREAD_CREATE));
	return (1);
}

int	main(int ac, char **av)
{
	t_tab	tab;

	tab.n_times_eaten = NULL;
	if (ac < 5 || ac > 6)
	{
		set_exit_code(&tab, ERROR_AC);
		return (exit_error(&tab));
	}
	if (!initialize_variables(&tab, ac, av)
		|| !create_philosophers(&tab)
		|| !monitor_philosophers(&tab))
		return (exit_error(&tab));
	wrap_up(&tab);
	return (0);
}
