#include "../philo_two.h"

void	put_status_msg(t_tab *tab, t_thread_var_struct *s, char *msg)
{
	if (!tab->phi_died && !tab->error_code)
		printf("%lld %d %s\n",
			(tab->current_time - tab->start_time), s->phi_n + 1, msg);
}

int	exit_error(t_tab *tab)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (tab->error_code == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (tab->error_code == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (tab->error_code == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (tab->error_code == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (tab->error_code == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (tab->error_code == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (tab->error_code == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (tab->error_code == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (tab->error_code == ERROR_SEM_WAIT)
		write(2, "sem_wait() returned -1\n", 24);
	else if (tab->error_code == ERROR_SEM_POST)
		write(2, "sem_post() returned -1\n", 24);
	wrap_up(tab);
	return (1);
}

void	*set_error_code(t_tab *tab, int error_code)
{
	tab->error_code = error_code;
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
	usleep(1000);
	free_malloced_variables(tab);
	sem_unlink("fork_availability");
	sem_unlink("starving_sem");
	sem_unlink("id_sem");
}
