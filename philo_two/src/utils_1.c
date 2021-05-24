#include "../philo_two.h"

int	abort_eating(t_tab *tab, sem_t *sem, int return_value, int exit_code)
{
	if (sem_post(sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_POST));
	if (exit_code)
		set_exit_code(tab, exit_code);
	return (return_value);
}

static void	eat_or_die(t_tab *tab, int phi_n, long long timestamp)
{
	if (tab->time_last_meal[phi_n] + tab->time_to_die < tab->current_time)
	{
		tab->exit_code = DEATH;
		printf("%lld %d "B_RED"died"RESET"\n",
			(tab->current_time - tab->start_time), phi_n + 1);
	}
	else
	{
		tab->time_last_meal[phi_n] = tab->current_time;
		printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
			timestamp, phi_n + 1, timestamp, phi_n + 1);
		printf("%lld %d is eating\n", timestamp, phi_n + 1);
	}
}

int	put_status_msg(t_tab *tab, int phi_n, char *msg)
{
	int			ret;
	long long	timestamp;

	if (sem_wait(tab->print_sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
	ret = 1;
	if (!tab->exit_code)
	{
		timestamp = tab->current_time - tab->start_time;
		if (msg[0] == 'e')
			eat_or_die(tab, phi_n, timestamp);
		else
			printf("%lld %d %s\n", timestamp, phi_n + 1, msg);
	}
	else
		ret = 0;
	if (sem_post(tab->print_sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
	return (ret);
}

void	*set_exit_code(t_tab *tab, int exit_code)
{
	if (tab->exit_code == 0
		|| tab->exit_code == DEATH
		|| tab->exit_code == ALL_FAT)
		tab->exit_code = exit_code;
	return (NULL);
}

int	wrap_up(t_tab *tab)
{
	int	ret;

	ret = 1;
	if (tab->pthreads_created)
		if (pthread_join(tab->philosopher_thread, NULL) != 0)
			ret = ((int)set_exit_code(tab, ERROR_PTHREAD_JOIN));
	if (tab->n_times_eaten)
	{
		free(tab->n_times_eaten);
		tab->n_times_eaten = NULL;
	}
	sem_unlink("fork_sem");
	sem_unlink("id_sem");
	sem_unlink("print_sem");
	return (ret);
}
