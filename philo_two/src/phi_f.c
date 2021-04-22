#include "../philo_two.h"

static void	*starving(t_tab *tab, t_thread_var_struct *s)
{
	put_status_msg(tab, s, B_RED"died"RESET);
	tab->phi_died = 1;
	return (NULL);
}

static int	sated(t_tab *tab, t_thread_var_struct *s)
{
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		>= tab->number_of_times_each_philosopher_must_eat)
	{
		put_status_msg(tab, s, B_GREEN"is fat enough"RESET);
		if (sem_post(tab->fork_availability) == -1)
			return ((int)return_error(tab, ERROR_SEM_POST));
		return (0);
	}
	return (1);
}

static int	eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;

	if (sem_wait(tab->fork_availability) == -1)
		return ((int)return_error(tab, ERROR_SEM_WAIT));
	if (s->time_last_meal + tab->time_to_die <= tab->current_time)
		return ((int)starving(tab, s));
	put_status_msg(tab, s, "has taken a fork");
	put_status_msg(tab, s, "has taken a fork");
	put_status_msg(tab, s, "is eating");
	s->time_last_meal = tab->current_time;
	time_done_eating = tab->current_time + tab->time_to_eat;
	while (time_done_eating > tab->current_time)
		if (usleep(1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
	tab->n_times_eaten[s->phi_n]++;
	return (sated(tab, s));
}

static int	sleeping(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	put_status_msg(tab, s, "is sleeping");
	if (sem_post(tab->fork_availability) == -1)
		return ((int)return_error(tab, ERROR_SEM_POST));
	waking_time = tab->current_time + tab->time_to_sleep;
	while (waking_time > tab->current_time)
	{
		if (s->time_last_meal + tab->time_to_die <= tab->current_time)
			return ((int)starving(tab, s));
		if (usleep(1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
	}
	return (1);
}

void	*phi_f(void *arg)
{
	t_tab				*tab;
	t_thread_var_struct	s;

	tab = (t_tab *)arg;
	tab->current_time = get_current_time(tab);
	if (tab->current_time == -1)
		return (return_error(tab, ERROR_GETTIMEOFDAY));
	s.time_last_meal = tab->current_time;
	s.phi_n = tab->phi_n;
	while (1)
	{
		if (s.time_last_meal + tab->time_to_die <= tab->current_time)
			return (starving(tab, &s));
		if (!eating(tab, &s))
			return (NULL);
		if (!sleeping(tab, &s))
			return (NULL);
		put_status_msg(tab, &s, "is thinking");
	}
}
