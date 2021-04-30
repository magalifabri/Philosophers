#include "../philo_one.h"

int	check_vitality(t_tab *tab, t_thread_var_struct *s)
{
	if (s->time_last_meal + tab->time_to_die <= tab->current_time)
	{
		if (pthread_mutex_lock(&tab->put_status_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
		if (!tab->exit_code)
		{
			tab->exit_code = DEATH;
			printf("%lld %d "B_RED"died"RESET"\n",
				(tab->current_time - tab->start_time), s->phi_n + 1);
		}
		if (pthread_mutex_unlock(&tab->put_status_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
		return (0);
	}
	return (1);
}

/*
Simply usleep()ing for time_to_eat * 1000 didn't work, because usleep()
would take a couple extra microseconds to return, causing small but
potentially fatal delays.
*/

int	mutex_unlock__return_0(t_tab *tab, pthread_mutex_t *lock_1,
	pthread_mutex_t *lock_2, int return_value)
{
	if (lock_1)
		if (pthread_mutex_unlock(lock_1) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	if (lock_2)
		if (pthread_mutex_unlock(lock_2) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	return (return_value);
}

static int	check_fatness(t_tab *tab, t_thread_var_struct *s)
{
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		== tab->number_of_times_each_philosopher_must_eat)
	{
		if (pthread_mutex_lock(&tab->put_status_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
		if (!tab->exit_code)
		{
			printf("%lld %d "B_GREEN"is fat"RESET"\n",
				(tab->current_time - tab->start_time), s->phi_n + 1);
			tab->number_of_fat_philosophers++;
			if (tab->number_of_fat_philosophers == tab->number_of_philosophers)
				tab->exit_code = ALL_FAT;
		}
		if (pthread_mutex_unlock(&tab->put_status_lock) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	}
	return (1);
}

/*
One big usleep() isn't used when the philosopher is sleeping, because the
philosopher might die in its sleep, and we need to be able to check that.
*/

static int	sleeping_thinking(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	if (!check_fatness(tab, s))
		return (0);
	waking_time = tab->current_time + tab->time_to_sleep;
	if (!put_status(tab, s->phi_n + 1, "is sleeping"))
		return (0);
	while (waking_time > tab->current_time && !tab->exit_code)
		if (usleep(500) == -1)
			return ((int)set_exit_code(tab, ERROR_USLEEP));
	// if (tab->exit_code)
	// 	return (0);
	if (!put_status(tab, s->phi_n + 1, "is thinking"))
		return (0);
	return (1);
}

static int	eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;
	
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_lock(&tab->forks[s->left_fork_i].lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
	s->time_last_meal = tab->current_time;
	time_done_eating = tab->current_time + tab->time_to_eat;
	if (!put_status(tab, s->phi_n + 1, "e"))
		return (mutex_unlock__return_0(tab, &tab->forks[s->phi_n].lock,
			&tab->forks[s->left_fork_i].lock, 0));
	while (time_done_eating > tab->current_time && !tab->exit_code)
		if (usleep(500) == -1)
			return (mutex_unlock__return_0(tab, &tab->forks[s->phi_n].lock,
				&tab->forks[s->left_fork_i].lock, 0));
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1
		|| pthread_mutex_unlock(&tab->forks[s->left_fork_i].lock) == -1)
		return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
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
**     is occurred via the variable tab.exit_code. In both cases, NULL
**     is returned. When a philosopher has reached
**     number_of_times_each_philosopher_must_eat, it also returns NULL.
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
	while (eating(tab, &s)
		&& sleeping_thinking(tab, &s))
		;
	pthread_join(grimreaper_thread, NULL);
	return (NULL);
}
