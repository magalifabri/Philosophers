#include "../philo_one.h"

/*
put_status() prints the philosphers' activities to stdout.
It uses the mutex lock put_status_lock to make sure only one philosopher
(thread) does this at a time.
It also ensures that no more status messages are printed when a philosopher
has dies or an error has occurred.
*/

int	put_status(t_tab *tab, int philo_n, char *msg)
{
	if (pthread_mutex_lock(tab->put_status_lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_LOCK));
	if (!tab->phi_died && !tab->error_code)
		printf("%lld %d %s\n",
			(tab->current_time - tab->start_time), philo_n, msg);
	if (pthread_mutex_unlock(tab->put_status_lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

int	check_vitality(t_tab *tab, t_thread_var_struct *s)
{
	if (s->time_last_meal + tab->time_to_die <= tab->current_time)
	{
		if (!put_status(tab, s->phi_n + 1, B_RED"died"RESET))
			return (0);
		tab->phi_died = 1;
		return (0);
	}
	return (1);
}

long long	get_current_time(t_tab *tab)
{
	struct timeval	tp;
	long long		passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)set_error_code(tab, ERROR_GETTIMEOFDAY));
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}

void	free_malloced_variables(t_tab *tab)
{
	if (tab->forks)
		free(tab->forks);
	if (tab->n_times_eaten)
		free(tab->n_times_eaten);
	// if (tab->phi_t)
	// 	free(tab->phi_t);
	if (tab->put_status_lock)
		free(tab->put_status_lock);
}

/*
** Note(s) on destroy_locks():
** 
** Reason for usleep(): give threads the time to exit (so no mutex locks are
** being used) before destroying the mutexes.
*/

int	destroy_locks(t_tab *tab)
{
	int	i;

	usleep(10000);
	i = -1;
	while (++i < tab->number_of_philosophers)
		if (pthread_mutex_destroy(&tab->forks[i].lock) != 0)
			return ((int)set_error_code(tab, ERROR_MUTEX_DESTROY));
	if (pthread_mutex_destroy(tab->put_status_lock) != 0)
		return ((int)set_error_code(tab, ERROR_MUTEX_DESTROY));
	return (1);
}
