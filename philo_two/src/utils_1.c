#include "../philo_two.h"

int	abort_eating(t_tab *tab, sem_t *sem, int return_value, int exit_code)
{
	if (sem_post(sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_POST));
	if (exit_code)
		set_exit_code(tab, exit_code);
	return (return_value);
}

int	put_status_msg(t_tab *tab, t_thread_var_struct *s, char *msg)
{
	int			ret;
	long long	timestamp;

	ret = 1;
	if (sem_wait(tab->print_sem) == -1)
		return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
	if (!tab->exit_code)
	{
		timestamp = tab->current_time - tab->start_time;
		if (msg[0] == 'e')
		{
			printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
				timestamp, s->phi_n + 1, timestamp, s->phi_n + 1);
			printf("%lld %d is eating\n", timestamp, s->phi_n + 1);
		}
		else
			printf("%lld %d %s\n", timestamp, s->phi_n + 1, msg);
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

void	free_malloced_variables(t_tab *tab)
{
	if (tab->n_times_eaten)
	{
		free(tab->n_times_eaten);
		tab->n_times_eaten = NULL;
	}
}

void	wrap_up(t_tab *tab)
{
	if (tab->pthreads_created)
		if (pthread_join(tab->philosopher_thread, NULL) != 0)
			printf(B_RED"ERROR: "RESET"pthread_join() didn't return 0\n");
	free_malloced_variables(tab);
	sem_unlink("fork_sem");
	sem_unlink("id_sem");
	sem_unlink("print_sem");
}
