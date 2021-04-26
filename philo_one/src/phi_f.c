#include "../philo_one.h"

static int	grab_forks_if_available(t_tab *tab, t_thread_var_struct *s)
{
	int	left_fork_i;

	if (s->phi_n == 0)
		left_fork_i = tab->number_of_philosophers - 1;
	else
		left_fork_i = s->phi_n - 1;
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_lock(&tab->forks[left_fork_i].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_LOCK));
	if (tab->forks[s->phi_n].available == 1
		&& tab->forks[left_fork_i].available == 1)
	{
		tab->forks[s->phi_n].available = 0;
		if (!put_status(tab, s->phi_n + 1, "has taken a fork"))
			return (0);
		tab->forks[left_fork_i].available = 0;
		if (!put_status(tab, s->phi_n + 1, "has taken a fork"))
			return (0);
		s->got_forks = 1;
	}
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_unlock(&tab->forks[left_fork_i].lock) == -1)
		return ((int)set_error_code(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

/*
When the number of philosophers is uneven, the eating done in three shifts.
Depending on the program's parameters, it can be possible that one
philosopher eats while another is in greater need. To try and remedy this
and keep the eating to a steady rhythm, we make sure that each philosopher
waits in between meals for an amount of time that allows all other
philosophers to have a meal as well. Otherwise it is almost certain that at
some point the one philosopher that eats in the third shift, will miss its
chance and drop dead.
*/

static int	queue(t_tab *tab, t_thread_var_struct *s)
{
	while (s->time_last_meal + (tab->time_to_eat * 2) + 5 > tab->current_time)
		if (usleep(1000) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
	if (!check_vitality(tab, s))
		return (0);
	return (1);
}

/*
Simply usleep()ing for time_to_eat * 1000 didn't work, because usleep()
would take a couple extra microseconds to return, causing small but
potentially fatal delays.
*/

static int	thinking_to_eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;

	if (tab->n_times_eaten[s->phi_n] != 0
		&& tab->number_of_philosophers % 2 != 0)
		if (!queue(tab, s))
			return (0);
	if (!grab_forks_if_available(tab, s))
		return (0);
	if (s->got_forks)
	{
		s->phi_state = 'e';
		s->time_last_meal = tab->current_time;
		if (!put_status(tab, s->phi_n + 1, "is eating"))
			return (0);
		time_done_eating = tab->current_time + tab->time_to_eat;
		while (time_done_eating > tab->current_time)
			if (usleep(1000) == -1)
				return ((int)set_error_code(tab, ERROR_USLEEP));
	}
	if (tab->phi_died || tab->error_code)
		return (0);
	return (1);
}

/*
** Note(s) on phi_f()
** 
** parameters: void *arg: t_tab *tab in void* form
** 
** description: This is the function the threads are send to. The while loop
**     functions as the routine of each philosopher with a cycle of thinking,
**     eating (if forks are available) and sleeping.
** 
** return values: When a philosopher dies, tab.phi_died is set to 1, which
**     signals to the other threads and the main process, that a philosopher
**     has died and things ought to be wrapped up. The same happens if an error
**     is occurred via the variable tab.error_code. In both cases, NULL
**     is returned. When a philosopher has reached
**     number_of_times_each_philosopher_must_eat, it also returns NULL.
*/

void	*phi_f(void *arg)
{
	t_tab				*tab;
	t_thread_var_struct	s;

	tab = (t_tab *)arg;
	initialize_variables_phi_f(tab, &s);
	while (1)
	{
		if (!check_vitality(tab, &s))
			return (NULL);
		if (s.phi_state == 't')
			if (!thinking_to_eating(tab, &s))
				return (NULL);
		if (s.phi_state == 'e')
			if (!eating_to_thinking(tab, &s))
				return (NULL);
	}
}
