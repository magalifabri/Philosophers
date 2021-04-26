#include "../philo_two.h"

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

// static int	check_if_all_are_fat(t_tab *tab)
// {
// 	int	i;
// 	int	number_of_fat_philosophers;

// 	i = -1;
// 	number_of_fat_philosophers = 0;
// 	while (++i < tab->number_of_philosophers)
// 	{
// 		if (tab->n_times_eaten[i]
// 			== tab->number_of_times_each_philosopher_must_eat)
// 			number_of_fat_philosophers++;
// 		if (number_of_fat_philosophers == tab->number_of_philosophers)
// 		{
// 			printf(B_GREEN"They're all fat. Good job!\n"RESET);
// 			return (1);
// 		}
// 	}
// 	return (0);
// }

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
			return ((int)set_error_code(tab, ERROR_USLEEP));
		tab->current_time = get_current_time();
		if (tab->current_time == -1)
			return ((int)set_error_code(tab, ERROR_GETTIMEOFDAY));
		if (tab->error_code)
			return (0);
		if (tab->phi_died == 1)
		{
			printf(B_RED"A philosopher has starved! Game over."RESET"\n");
			return (1);
		}
		if (tab->all_fat)
			return (1);
		// if (check_if_all_are_fat(tab))
		// 	return (1);
	}
	return (0);
}

static int	create_philosophers(t_tab *tab)
{
	pthread_t	philosopher_thread;
	int			i;

	tab->phi_n_c = 0;
	i = -1;
	tab->current_time = get_current_time();
	if (tab->current_time == -1)
		return ((int)set_error_code(tab, ERROR_GETTIMEOFDAY));
	while (++i < tab->number_of_philosophers
		&& !tab->error_code && !tab->phi_died)
		if (pthread_create(&philosopher_thread, NULL, phi_f, tab) != 0)
			return ((int)set_error_code(tab, ERROR_PTHREAD_CREATE));
	pthread_detach(philosopher_thread);
	return (1);
}

int	main(int ac, char **av)
{
	t_tab	tab;

	tab.n_times_eaten = NULL;
	if (ac < 5 || ac > 6)
	{
		set_error_code(&tab, ERROR_AC);
		return (exit_error(&tab));
	}
	if (!initialize_variables(&tab, ac, av)
		|| !create_philosophers(&tab)
		|| !monitor_philosophers(&tab))
		return (exit_error(&tab));
	wrap_up(&tab);
	return (0);
}
