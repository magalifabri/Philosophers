#include "../philo_two.h"

static int	sleeping_thinking(t_tab *tab, t_thread_var_struct *s)
{
	long long	waking_time;

	waking_time = tab->current_time + tab->time_to_sleep;
	if (!put_status_msg(tab, s, "is sleeping"))
		return (0);
	while (waking_time > tab->current_time && !tab->exit_code)
		if (usleep(1000) == -1)
			return ((int)set_exit_code(tab, ERROR_USLEEP));
	if (!put_status_msg(tab, s, "is thinking"))
		return (0);
	return (1);
}

static int	check_fatness(t_tab *tab, t_thread_var_struct *s)
{
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		== tab->number_of_times_each_philosopher_must_eat)
	{
		if (sem_wait(tab->print_sem) == -1)
			return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
		if (!tab->exit_code)
		{
			printf("%lld %d "B_GREEN"is fat"RESET"\n",
				(tab->current_time - tab->start_time), s->phi_n + 1);
			tab->number_of_fat_philosophers++;
			if (tab->number_of_fat_philosophers == tab->number_of_philosophers)
				tab->exit_code = ALL_FAT;
		}
		if (sem_post(tab->print_sem) == -1)
			return ((int)set_exit_code(tab, ERROR_SEM_POST));
	}
	return (1);
}

static int	eating(t_tab *tab, t_thread_var_struct *s)
{
	long long	time_done_eating;

	if (sem_wait(tab->fork_sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
	time_done_eating = tab->current_time + tab->time_to_eat;
	if (!put_status_msg(tab, s, "e"))
		return (abort_eating(tab, tab->fork_sem, 0, 0));
	while (time_done_eating > tab->current_time && !tab->exit_code)
		if (usleep(1000) == -1)
			return (abort_eating(tab, tab->fork_sem, 0, ERROR_USLEEP));
	if (sem_post(tab->fork_sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_POST));
	if (!check_fatness(tab, s))
		return (0);
	return (1);
}

void	*phi_f(void *arg)
{
	t_tab				*tab;
	t_thread_var_struct	*s;

	tab = (t_tab *)arg;
	s = malloc(sizeof(t_thread_var_struct));
	if (!s)
		return (set_exit_code(tab, ERROR_MALLOC));
	if (sem_wait(tab->id_sem) == -1)
		return (set_exit_code(tab, ERROR_SEM_WAIT));
	s->phi_n = tab->phi_n_c++;
	if (sem_post(tab->id_sem) == -1)
		return (set_exit_code(tab, ERROR_SEM_POST));
	tab->time_last_meal[s->phi_n] = tab->start_time;
	s->tab = tab;
	while (eating(tab, s) && sleeping_thinking(tab, s))
		;
	return (NULL);
}
