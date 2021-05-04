#include "../philo_one.h"

/*
Helper function to trim down the length of put_status().
It checks when a philosopher is about to eat, if it shouldn't be dead instead.
*/

void	eat_or_die(t_tab *tab, t_thread_var_struct *s, long long timestamp)
{
	if (s->time_last_meal + s->tab->time_to_die < s->tab->current_time)
	{
		s->tab->exit_code = DEATH;
		printf("%lld %d "B_RED"died"RESET"\n",
			(s->tab->current_time - s->tab->start_time), s->phi_n + 1);
	}
	else
	{
		s->time_last_meal = tab->current_time;
		printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
			timestamp, s->phi_n + 1, timestamp, s->phi_n + 1);
		printf("%lld %d is eating\n", timestamp, s->phi_n + 1);
	}
}

/*
put_status() prints the philosophers' activities to stdout. It uses a mutex
lock to make sure only one philosopher (thread) does this at a time. It also
prevents more status messages from being printed when a philosopher
has died, an error has occurred or all philosophers are fat and reports this
back to the caller function as this means the threads need to exit.
*/

int	put_status(t_tab *tab, t_thread_var_struct *s, char *msg)
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
			eat_or_die(tab, s, timestamp);
		else
			printf("%lld %d %s\n", timestamp, s->phi_n + 1, msg);
	}
	else
		ret = 0;
	if (pthread_mutex_unlock(&tab->print_lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	return (ret);
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
	free_malloced_variables(tab);
	return (ret);
}
