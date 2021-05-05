#include "../philo_one.h"

static void	*grimreaper(void *arg)
{
	t_thread_var_struct	*s;

	s = (t_thread_var_struct *)arg;
	while (!s->tab->exit_code)
	{
		if (s->time_last_meal + s->tab->time_to_die < s->tab->current_time)
		{
			if (pthread_mutex_lock(&s->tab->print_lock) == -1)
				return (set_exit_code(s->tab, ERROR_MUTEX_LOCK));
			if (!s->tab->exit_code)
			{
				s->tab->exit_code = DEATH;
				printf("%lld %d "B_RED"died!"RESET"\n",
					(s->tab->current_time - s->tab->start_time), s->phi_n + 1);
			}
			if (pthread_mutex_unlock(&s->tab->print_lock) == -1)
				return (set_exit_code(s->tab, ERROR_MUTEX_UNLOCK));
		}
		if (usleep(1000) == -1)
			return (set_exit_code(s->tab, ERROR_USLEEP));
	}
	return (NULL);
}

/*
For whatever reason one big usleep() is slower than a bunch of small ones.
*/

static int	sleeping_thinking(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	waking_time = tab->current_time + tab->time_to_sleep;
	if (!put_status(tab, s, "is sleeping"))
		return (0);
	while (waking_time > tab->current_time && !tab->exit_code)
		if (usleep(500) == -1)
			return ((int)set_exit_code(tab, ERROR_USLEEP));
	if (!put_status(tab, s, "is thinking"))
		return (0);
	return (1);
}

/*
Function to check if:
1. this thread's philosopher has eaten the number of times specified in the
program's (optional) argument, and if so:
2. all the philosophers have eaten this number of times, in which case the
program exits
*/

static int	check_fatness(t_tab *tab, t_thread_var_struct *s)
{
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		== tab->number_of_times_each_philosopher_must_eat)
	{
		if (pthread_mutex_lock(&tab->print_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
		if (!tab->exit_code)
		{
			printf("%lld %d "B_GREEN"is fat"RESET"\n",
				(tab->current_time - tab->start_time), s->phi_n + 1);
			tab->number_of_fat_philosophers++;
			if (tab->number_of_fat_philosophers == tab->number_of_philosophers)
				tab->exit_code = ALL_FAT;
		}
		if (pthread_mutex_unlock(&tab->print_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	}
	return (1);
}

static int	eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;

	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_lock(&tab->forks[s->left_fork_i].lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
	time_done_eating = tab->current_time + tab->time_to_eat;
	if (!put_status(tab, s, "e"))
		return (mutex_unlock__return_0(tab, &tab->forks[s->phi_n].lock,
				&tab->forks[s->left_fork_i].lock, 0));
	while (time_done_eating > tab->current_time && !tab->exit_code)
		if (usleep(500) == -1)
			return (mutex_unlock__return_0(tab, &tab->forks[s->phi_n].lock,
					&tab->forks[s->left_fork_i].lock, 0));
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_unlock(&tab->forks[s->left_fork_i].lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
	if (!check_fatness(tab, s))
		return (0);
	return (1);
}

/*
Note on:
	if (s.phi_n % 2 == 0)
		usleep(5000);
The philosophers eat in two turns. Half the philosophers (every other
one) is halted for just a moment at the starting line. This is to ensure
no conflicts occur surrounding the forks' mutex locks and they get off
to a time efficient start.

Note on grimreaper_thread: When a philosopher thread is waiting for a mutex
lock to become available, it can't do anything else in the meantime. Thus it
also can't check if the philosopher has died and report on its death in a
timely manner. This task is outsourced to a separate thread: grimreaper.
This thread continually checks if the philosopher is still alive and reports
on their death when required.
*/

void	*phi_f(void *arg)
{
	t_tab				*tab;
	t_thread_var_struct	s;
	pthread_t			grimreaper_thread;

	tab = (t_tab *)arg;
	if (!initialize_variables_phi_f(tab, &s))
		return (NULL);
	if (pthread_create(&grimreaper_thread, NULL, grimreaper, &s) != 0)
		return (NULL);
	if (s.phi_n % 2 == 0)
		usleep(5000);
	while (eating(tab, &s) && sleeping_thinking(tab, &s))
		;
	pthread_join(grimreaper_thread, NULL);
	return (NULL);
}
