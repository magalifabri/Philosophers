#include "../philo_two.h"

void *starving(t_tab *tab, t_thread_var_struct *s)
{
	if (!put_status_msg(tab, (tab->current_time - tab->start_time)
	, s->phi_n + 1, B_RED"died\n"RESET))
		return (return_error(tab, ERROR_MALLOC));
	tab->phi_died = 1;
	return (NULL);
}

int eating(t_tab *tab, t_thread_var_struct *s)
{
	if (sem_wait(tab->fork_availability) == -1)
		return ((int)return_error(tab, ERROR_SEM_WAIT));
	if (s->time_last_meal + tab->time_to_die <= tab->current_time)
		return ((int)starving(tab, s));
	if (!put_status_msg(tab, (tab->current_time - tab->start_time)
	, s->phi_n + 1, "is eating\n"))
		return ((int)return_error(tab, ERROR_MALLOC));
	s->time_last_meal = tab->current_time;
	if (usleep(tab->time_to_eat * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1 &&
	tab->n_times_eaten[s->phi_n]
	>= tab->number_of_times_each_philosopher_must_eat)
	{
		if (!(put_status_msg(tab, (tab->current_time - tab->start_time),
		s->phi_n + 1, B_GREEN"is fat enough\n"RESET)))
			return ((int)return_error(tab, ERROR_MALLOC));
		if (sem_post(tab->fork_availability) == -1)
			return ((int)return_error(tab, ERROR_SEM_POST));
		return (0);
	}
	return (1);
}

int sleeping(t_tab *tab, t_thread_var_struct *s)
{
	if (!put_status_msg(tab, (tab->current_time - tab->start_time)
	, s->phi_n + 1, "is sleeping\n"))
		return ((int)return_error(tab, ERROR_MALLOC));
	if (sem_post(tab->fork_availability) == -1)
		return ((int)return_error(tab, ERROR_SEM_POST));
	if (usleep(tab->time_to_sleep * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	return (1);
}

void *phi_f(void *arg)
{
	t_tab *tab;
	t_thread_var_struct s;
	
	tab = (t_tab *)arg;
	if ((tab->current_time = get_current_time(tab)) == -1)
		return (return_error(tab, ERROR_GETTIMEOFDAY));
	s.time_last_meal = tab->current_time;
	s.phi_n = tab->phi_n;
	while(1)
	{
		if (s.time_last_meal + tab->time_to_die <= tab->current_time)
			return (starving(tab, &s));
		if (!eating(tab, &s))
			return (NULL);
		if (!sleeping(tab, &s))
			return (NULL);
		if (!put_status_msg(tab, (tab->current_time - tab->start_time)
		, s.phi_n + 1, "is thinking\n"))
			return (return_error(tab, ERROR_MALLOC));
	}
}
