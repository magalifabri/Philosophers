#include "../philo_one.h"

typedef struct s_phi_t_variable_struct
{
	int phi_n; // sentinel value for an uninitialized philosopher
	int left_fork_held;
	int right_fork_held;
	int phi_state;
	long long time_last_meal;
	long long time_sleep_start;
} t_phi_t_variable_struct;

// THINKING TO EATING

int grab_right_fork_if_available(t_tab *tab, t_phi_t_variable_struct *s)
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

int grab_left_fork_if_available_1(t_tab *tab, t_phi_t_variable_struct *s)
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

int grab_left_fork_if_available_2(t_tab *tab, t_phi_t_variable_struct *s)
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

int thinking_to_eating(t_tab *tab, t_phi_t_variable_struct *s)
{
	grab_right_fork_if_available(tab, s);
	if (s->phi_n == 0)
		grab_left_fork_if_available_1(tab, s);
	else
		grab_left_fork_if_available_2(tab, s);
	if (s->left_fork_held && s->right_fork_held)
	{
		s->phi_state = 'e';
		s->time_last_meal = tab->current_time;
		if (!put_status_msg((tab->current_time - tab->start_time)
		, s->phi_n + 1, "is eating\n"))
			return (0);
		if (usleep(tab->time_to_eat * 1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
	}
	return (1);
}

// EATING TO SLEEPING

int lay_down_forks(t_tab *tab, t_phi_t_variable_struct *s)
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

int finish_eating_and_obesity_check(t_tab *tab, t_phi_t_variable_struct *s)
{
	if (!lay_down_forks(tab, s))
		return (0);
	s->right_fork_held = 0;
	s->left_fork_held = 0;
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

int eating_to_thinking(t_tab *tab, t_phi_t_variable_struct *s)
{
	if (!finish_eating_and_obesity_check(tab, s))
		return (0);
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "is sleeping\n"))
		return (0);
	if (usleep(tab->time_to_sleep * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	s->phi_state = 't';
	if (!put_status_msg((tab->current_time - tab->start_time)
	, s->phi_n + 1, "is thinking\n"))
		return (0);
	return (1);
}

static void initialize_variables_phi_f(t_tab *tab, t_phi_t_variable_struct *s)
{
	s->phi_n = tab->phi_n;
	s->left_fork_held = 0;
	s->right_fork_held = 0;
	s->phi_state = 't';
	s->time_sleep_start = tab->current_time;
	s->time_last_meal = tab->current_time;
}

void *phi_f(void *arg)
{
	t_tab *tab;
	t_phi_t_variable_struct s;
	
	tab = (t_tab *)arg;
	initialize_variables_phi_f(tab, &s);
	while (1)
	{
		if (s.time_last_meal + tab->time_to_die <= tab->current_time)
		{
			tab->phi_died = 1;
			put_status_msg((tab->current_time - tab->start_time)
			, s.phi_n + 1, B_RED"died\n"RESET);
			return (NULL);
		}
		if (s.phi_state == 't' && !thinking_to_eating(tab, &s))
			return (NULL);
		if (s.phi_state == 'e'
		&& s.time_last_meal + tab->time_to_eat <= tab->current_time
		&& !eating_to_thinking(tab, &s))
			return (NULL);
		if (usleep(5000) == -1)
			return (return_error(tab, ERROR_USLEEP));
	}
}
