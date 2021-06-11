#include "../philo_one.h"

/*
Helper function to trim down the length of put_status().
It checks when a philosopher is about to eat, if it shouldn't be dead instead.
*/

static void	eat_or_die(t_tab *tab, int phi_n, long long timestamp)
{
	if (tab->time_last_meal[phi_n] + tab->time_to_die < tab->current_time)
	{
		tab->exit_code = DEATH;
		printf("%lld %d "B_RED"died"RESET"\n",
			(tab->current_time - tab->start_time), phi_n + 1);
	}
	else
	{
		tab->time_last_meal[phi_n] = tab->current_time;
		printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
			timestamp, phi_n + 1, timestamp, phi_n + 1);
		printf("%lld %d is eating\n", timestamp, phi_n + 1);
	}
}

/*
put_status() prints the philosophers' activities to stdout. It uses a mutex
lock to make sure only one philosopher (thread) does this at a time. It also
prevents more status messages from being printed when a philosopher
has died, an error has occurred or all philosophers are fat and reports this
back to the caller function as this means the threads need to exit.
*/

int	put_status(t_tab *tab, int phi_n, char *msg)
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
			eat_or_die(tab, phi_n, timestamp);
		else
			printf("%lld %d %s\n", timestamp, phi_n + 1, msg);
	}
	else
		ret = 0;
	if (pthread_mutex_unlock(&tab->print_lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	return (ret);
}

/*
A helper function, usually called in a return statement, that allows us to set
an exit code, indicating the reason for exiting the process, and return 0.

Only set or replace the currently stored exit code if it's 0 (initialisation
value) or an exit code that doesn't indicate an error (DEATH or ALL_FAT).
Otherwise, if the currently stored exit code already indicates an error,
don't replace it, as the initial error is the most important.
*/

void	*set_exit_code(t_tab *tab, int exit_code)
{
	if (tab->exit_code == 0
		|| tab->exit_code == DEATH
		|| tab->exit_code == ALL_FAT)
		tab->exit_code = exit_code;
	return (NULL);
}

static int	destroy_locks(t_tab *tab)
{
	int	ret;
	int	i;

	ret = 1;
	i = -1;
	while (++i < tab->n_fork_locks_initialized)
		if (pthread_mutex_destroy(&tab->forks[i].lock) != 0)
			ret = ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->print_lock_initialized)
		if (pthread_mutex_destroy(&tab->print_lock) != 0)
			ret = ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	if (tab->id_lock_initialized)
		if (pthread_mutex_destroy(&tab->id_lock) != 0)
			ret = ((int)set_exit_code(tab, ERROR_MUTEX_DESTROY));
	return (ret);
}

/*
Calls all the functions required to clean up before exiting.

Note on `usleep(10000);` gives threads the time to exit before starting
the clean up. Not doing so can result in errors.
*/

int	wrap_up(t_tab *tab)
{
	int	ret;

	ret = 1;
	if (usleep(10000) == -1)
		ret = ((int)set_exit_code(tab, ERROR_USLEEP));
	if (tab->pthreads_created)
		if (pthread_join(tab->philosopher_thread, NULL) != 0)
			ret = ((int)set_exit_code(tab, ERROR_PTHREAD_JOIN));
	if (!destroy_locks(tab))
		ret = 0;
	if (tab->forks)
		free(tab->forks);
	if (tab->n_times_eaten)
		free(tab->n_times_eaten);
	system("leaks philo_one > leaks_report.out");
	return (ret);
}
