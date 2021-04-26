#include "../philo_one.h"

static int	lay_down_forks(t_tab *tab, t_thread_var_struct *s)
{
	int	left_fork_i;

	if (s->phi_n == 0)
		left_fork_i = tab->number_of_philosophers - 1;
	else
		left_fork_i = s->phi_n - 1;
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_LOCK));
	tab->forks[s->phi_n].available = 1;
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_UNLOCK));
	if (pthread_mutex_lock(&tab->forks[left_fork_i].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_LOCK));
	tab->forks[left_fork_i].available = 1;
	if (pthread_mutex_unlock(&tab->forks[left_fork_i].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

static int	finish_eating_and_obesity_check(t_tab *tab, t_thread_var_struct *s)
{
	if (!lay_down_forks(tab, s))
		return (0);
	s->got_forks = 0;
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		>= tab->number_of_times_each_philosopher_must_eat)
		if (!put_status(tab, s->phi_n + 1, B_GREEN"is fat"RESET))
			return (0);
	return (1);
}

/*
One big usleep() isn't used when the philosopher is sleeping, because the
philosopher might die in its sleep, and we need to be able to check that.
*/

int	eating_to_thinking(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	if (!finish_eating_and_obesity_check(tab, s))
		return (0);
	if (!put_status(tab, s->phi_n + 1, "is sleeping"))
		return (0);
	waking_time = tab->current_time + tab->time_to_sleep;
	while (waking_time > tab->current_time)
	{
		if (!check_vitality(tab, s))
			return (0);
		if (usleep(1000) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
	}
	s->phi_state = 't';
	if (!put_status(tab, s->phi_n + 1, "is thinking"))
		return (0);
	return (1);
}
