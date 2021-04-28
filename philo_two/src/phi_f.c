#include "../philo_two.h"

void	*starving(t_tab *tab, t_thread_var_struct *s)
{
	if (sem_wait(tab->starving_sem) == -1)
		return (set_error_code(tab, ERROR_SEM_WAIT));
	if (!tab->all_fat && !tab->error_code)
	{
		tab->phi_died = 1;
		printf("%lld %d "B_RED"died"RESET"\n",
			(tab->current_time - tab->start_time), s->phi_n + 1);
	}
	return (NULL);
}

static int	eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;

	// printf("eating: %lld %d\n",
	// 	(tab->current_time - tab->start_time), s->phi_n + 1);
	if (sem_wait(tab->fork_availability) == -1)
		return ((int)set_error_code(tab, ERROR_SEM_WAIT));
	if (!put_status_msg(tab, s, "has taken a fork"))
		return (0);
	if (!put_status_msg(tab, s, "has taken a fork"))
		return (0);
	if (s->time_last_meal + tab->time_to_die <= tab->current_time)
		return ((int)starving(tab, s));
	s->time_last_meal = tab->current_time;
	if (!put_status_msg(tab, s, "is eating"))
		return (0);
	time_done_eating = tab->current_time + tab->time_to_eat;
	while (time_done_eating > tab->current_time
		&& !tab->phi_died && !tab->all_fat && !tab->error_code)
		if (usleep(1000) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
	if (tab->phi_died || tab->all_fat || tab->error_code)
		return (0);
	if (tab->n_times_eaten)
		tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten && tab->n_times_eaten[s->phi_n]
		== tab->number_of_times_each_philosopher_must_eat)
	{
		if (sem_wait(tab->put_status_msg_sem) == -1)
			return ((int)set_error_code(tab, ERROR_SEM_WAIT));
		// put_status_msg(tab, s, B_GREEN"is fat"RESET);
		printf("%lld %d "B_GREEN"is fat"RESET"\n",
			(tab->current_time - tab->start_time), s->phi_n + 1);
		tab->number_of_fat_philosophers++;
		if (tab->number_of_fat_philosophers == tab->number_of_philosophers)
		{
			tab->all_fat = 1;
			// printf(B_GREEN"They're all fat. Good job!\n"RESET);
		}
		if (sem_post(tab->put_status_msg_sem) == -1)
			return ((int)set_error_code(tab, ERROR_SEM_POST));
	}
	return (1);
}

static int	sleeping(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	// printf("sleeping: %lld %d\n",
	// 	(tab->current_time - tab->start_time), s->phi_n + 1);
	if (!put_status_msg(tab, s, "is sleeping"))
		return (0);
	if (sem_post(tab->fork_availability) == -1)
		return ((int)set_error_code(tab, ERROR_SEM_POST));
	waking_time = tab->current_time + tab->time_to_sleep;
	while (waking_time > tab->current_time
		&& !tab->phi_died && !tab->all_fat && !tab->error_code)
	{
		if (s->time_last_meal + tab->time_to_die <= tab->current_time)
			return ((int)starving(tab, s));
		if (usleep(1000) == -1)
			return ((int)set_error_code(tab, ERROR_USLEEP));
	}
	if (!put_status_msg(tab, s, "is thinking"))
		return (0);
	return (1);
}

void	*phi_f(void *arg)
{
	t_tab				*tab;
	t_thread_var_struct	*s;
	pthread_t			grimreaper_thread;

	tab = (t_tab *)arg;
	s = malloc(sizeof(t_thread_var_struct));
	if (!s)
		return (set_error_code(tab, ERROR_MALLOC));
	if (sem_wait(tab->id_sem) == -1)
		return (set_error_code(tab, ERROR_SEM_WAIT));
	s->phi_n = tab->phi_n_c++;
	if (sem_post(tab->id_sem) == -1)
		return (set_error_code(tab, ERROR_SEM_POST));
	s->time_last_meal = tab->current_time;
	s->tab = tab;
	if (pthread_create(&grimreaper_thread, NULL, grimreaper, s) != 0)
		return (NULL);
	pthread_detach(grimreaper_thread);
	while (1)
		if (!eating(tab, s) || !sleeping(tab, s))
			return (NULL);
}
