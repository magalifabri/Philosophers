#include "../philo_one.h"

static int lay_down_forks(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_LOCK));
	tab->forks[s->phi_n].available = 1;
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	if (s->phi_n == 0)
	{
		if (pthread_mutex_lock(
		&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_LOCK));
		tab->forks[tab->number_of_philosophers - 1].available = 1;
		if (pthread_mutex_unlock(
		&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	}
	else
	{
		if (pthread_mutex_lock(&tab->forks[s->phi_n - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_LOCK));
		tab->forks[s->phi_n - 1].available = 1;
		if (pthread_mutex_unlock(&tab->forks[s->phi_n - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	}
	return (1);
}

static int finish_eating_and_obesity_check(t_tab *tab, t_thread_var_struct *s)
{
	if (!lay_down_forks(tab, s))
		return (0);
	s->right_fork_held = 0;
	s->left_fork_held = 0;
	if (tab->phi_died || tab->error_encountered)
		return (0);
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "put his forks down\n"))
		return (0);
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
	&& tab->n_times_eaten[s->phi_n]
	>= tab->number_of_times_each_philosopher_must_eat)
	{
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, B_GREEN"is fat\n"RESET))
			return (0);
		return (0);
	}
	return (1);
}

/*
How to make sure no mutex locks are in use when they are destroyed?

If the program exits because all the philosophers are fat, then all the threads have already stopped and destroying unlocked mutex locks won't happen. But when a philosopher dies (or an error occurs) - and the other philosophers are still out and about doing their thing - it's important that the other philosophers have also exited, or are at least not unlocking mutexes, when the mutexes are destroyed.

The mutex locks are only in use for fractions of seconds with long gaps of usleep() in between. So if a thread checks tab->phi_died before and after usleep(), things should be fine (if we combine that with waiting for a millisecond or so after a philosopher has died before we destroy the mutex locks).

Checking whether a philosopher has died before a status message is posted, is also good, so that no stray messages pop up after a death.
These status messages coincide with the usleep() gaps, so a death (as well as) error check has been put in place before the status messages that 


*/

int eating_to_thinking(t_tab *tab, t_thread_var_struct *s)
{
	if (!finish_eating_and_obesity_check(tab, s))
		return (0);
	if (tab->phi_died || tab->error_encountered)
		return (0);
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "is sleeping\n"))
		return (0);
	if (usleep(tab->time_to_sleep * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	if (tab->phi_died || tab->error_encountered)
		return (0);
	s->phi_state = 't';
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "is thinking\n"))
		return (0);
	return (1);
}
