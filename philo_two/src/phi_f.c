#include "../philo_two.h"
/*
static int grab_right_fork_if_available(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_LOCK));
	if (tab->forks[s->phi_n].available == 1)
	{
		tab->forks[s->phi_n].available = 0;
		s->right_fork_held = 1;
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, "has taken a fork (right)\n"))
			return (0);
	}
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

static int grab_left_fork_if_available_1(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_LOCK));
	if (tab->forks[tab->number_of_philosophers - 1].available == 1)
	{
		tab->forks[tab->number_of_philosophers - 1].available = 0;
		s->left_fork_held = 1;
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, "has taken a fork (left)\n"))
			return (0);
	}
	if (pthread_mutex_unlock(&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

static int grab_left_fork_if_available_2(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->forks[s->phi_n - 1].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_LOCK));
	if (tab->forks[s->phi_n - 1].available == 1)
	{
		tab->forks[s->phi_n - 1].available = 0;
		s->left_fork_held = 1;
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, "has taken a fork (left)\n"))
			return (0);
	}
	if (pthread_mutex_unlock(&tab->forks[s->phi_n - 1].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	return (1);
}

static int thinking_to_eating(t_tab *tab, t_thread_var_struct *s)
{
	if (!grab_right_fork_if_available(tab, s))
		return (0);
	if (s->phi_n == 0 && !grab_left_fork_if_available_1(tab, s))
		return (0);
	else if (!grab_left_fork_if_available_2(tab, s))
		return (0);
	if (s->left_fork_held && s->right_fork_held)
	{
		s->phi_state = 'e';
		s->time_last_meal = tab->current_time;
		if (tab->phi_died || tab->error_encountered)
			return (0);
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, "is eating\n"))
			return (0);
		if (usleep(tab->time_to_eat * 1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
	}
	if (tab->phi_died || tab->error_encountered)
		return (0);
	return (1);
}


Note(s) on phi_f()

parameters:
	void *arg: t_tab *tab in void* form

description: 
	This is the function the threads are send to.
	The while loop functions as the routine of each philosopher with a cycle of thinking, eating (if forks are available) and sleeping.

return values:
	When a philosopher dies, tab.phi_died is set to 1, which signals to the other threads and the main process, that a philosopher has died and things ought to be wrapped up. The same happens if an error is occurred via the variable tab.error_encountered. In both cases, NULL is returned.
	When a philosopher has reached number_of_times_each_philosopher_must_eat, it also returns NULL.


void *phi_f(void *arg)
{
	t_tab *tab;
	t_thread_var_struct s;
	
	tab = (t_tab *)arg;
	initialize_variables_phi_f(tab, &s);
	while (1)
	{
		if (s.time_last_meal + tab->time_to_die <= tab->current_time)
		{
			put_status_msg((tab->current_time - tab->start_time)
			, s.phi_n + 1, B_RED"died\n"RESET);
			tab->phi_died = 1;
			return (NULL);
		}
		if (s.phi_state == 't' && !thinking_to_eating(tab, &s))
			return (NULL);
		if (s.phi_state == 'e'
		&& s.time_last_meal + tab->time_to_eat <= tab->current_time
		&& !eating_to_thinking(tab, &s))
			return (NULL);
	}
}
*/
// ----------------------------

typedef struct s_thread_variable_struct
{
	int phi_n;
	long long time_last_meal;
} t_thread_var_struct;

int waiting_and_eating(t_tab *tab, t_thread_var_struct *s)
{
	if (sem_wait(tab->fork_availability) == -1)
		return ((int)return_error(tab, ERROR_SEM_WAIT));
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "is eating\n"))
		return (0);
	s->time_last_meal = tab->current_time;
	if (usleep(tab->time_to_eat * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1 &&
	tab->n_times_eaten[s->phi_n] >= tab->number_of_times_each_philosopher_must_eat)
	{
		if (!(put_status_msg((tab->current_time - tab->start_time),
		s->phi_n + 1, B_GREEN"is fat enough\n"RESET)))
			return (0);
		if (sem_post(tab->fork_availability) == -1)
			return ((int)return_error(tab, ERROR_SEM_POST));
		return (0);
	}
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
		{
			put_status_msg((tab->current_time - tab->start_time)
			, s.phi_n + 1, B_RED"died\n"RESET);
			tab->phi_died = 1;
			return (NULL);
		}
		if (!waiting_and_eating(tab, &s))
			return (NULL);
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s.phi_n + 1, "is sleeping\n"))
			return (NULL);
		if (sem_post(tab->fork_availability) == -1)
			return (return_error(tab, ERROR_SEM_POST));
		if (usleep(tab->time_to_sleep * 1000) == -1)
			return (return_error(tab, ERROR_USLEEP));
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s.phi_n + 1, "is thinking\n"))
			return (NULL);
	}
}