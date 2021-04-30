#include "../philo_one.h"

/*
put_status() prints the philosophers' activities to stdout. It uses a mutex
lock to make sure only one philosopher (thread) does this at a time. It also
prevents more status messages from being printed when a philosopher
has died, an error has occurred or all philosophers are fat and reports this
back to the caller function as this means the threads need to exit.
*/

int	put_status(t_tab *tab, int philo_n, char *msg)
{
	int			ret;
	long long	timestamp;

	if (pthread_mutex_lock(&tab->print_lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
	ret = 1;
	if (!tab->exit_code)
	{
		timestamp = tab->current_time - tab->start_time;
		if (msg[0] == 'e')
		{
			printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
				timestamp, philo_n, timestamp, philo_n);
			printf("%lld %d is eating\n", timestamp, philo_n);
		}
		else
			printf("%lld %d %s\n", timestamp, philo_n, msg);
	}
	else
		ret = 0;
	if (pthread_mutex_unlock(&tab->print_lock) == -1)
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

int	destroy_locks(t_tab *tab)
{
	int	i;

	i = -1;
	while (++i < tab->n_fork_locks_initialized)
		if (pthread_mutex_destroy(&tab->forks[i].lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->print_lock_initialized)
		if (pthread_mutex_destroy(&tab->print_lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->id_lock_initialized)
		if (pthread_mutex_destroy(&tab->id_lock) != 0)
			return ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	return (1);
}

/*
Calls all the functions required to clean up before exiting.

Note on:
	usleep(10000);
gives threads the time to exit before starting the clean up. Not doing so can
result in errors.
*/

void	wrap_up(t_tab *tab)
{
	usleep(10000);
	pthread_join(tab->philosopher_thread, NULL);
	destroy_locks(tab);
	free_malloced_variables(tab);
}
