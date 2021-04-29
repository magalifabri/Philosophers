#include "../philo_one.h"

/*
put_status() prints the philosphers' activities to stdout. It uses a mutex
lock to make sure only one philosopher (thread) does this at a time. It also
tries to prevent more status messages from being printed when a philosopher
has died, an error has occurred or all philosophers are fat.
*/

int	put_status(t_tab *tab, int philo_n, char *msg)
{
	int ret;

	ret = 1;
	if (pthread_mutex_lock(&tab->put_status_lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
	if (!tab->exit_code)
		printf("%lld %d %s\n",
			(tab->current_time - tab->start_time), philo_n, msg);
	else
		ret = 0;
	if (pthread_mutex_unlock(&tab->put_status_lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	return (ret);
}

long long	get_current_time(t_tab *tab)
{
	struct timeval	tp;
	long long		passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)set_exit_code(tab, ERROR_GETTIMEOFDAY));
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
}

/*
Reason for usleep(): gives threads the time to exit (so no mutex locks are
being used) before destroying the mutexes.
*/

int	destroy_locks(t_tab *tab)
{
	int	i;

	i = -1;
	while (tab->forks[++i].lock_initialized)
		if (pthread_mutex_destroy(&tab->forks[i].lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->put_status_lock_initialized)
		if (pthread_mutex_destroy(&tab->put_status_lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->id_lock_initialized)
		if (pthread_mutex_destroy(&tab->id_lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	return (1);
}

void	wrap_up(t_tab *tab)
{
	usleep(10000);
	pthread_join(tab->philosopher_thread, NULL);
	destroy_locks(tab);
	free_malloced_variables(tab);
}
