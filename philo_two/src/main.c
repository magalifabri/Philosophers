#include "../philo_two.h"

static int	exit_error(t_tab *tab)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (tab->exit_code == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (tab->exit_code == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (tab->exit_code == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (tab->exit_code == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (tab->exit_code == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (tab->exit_code == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (tab->exit_code == ERROR_PTHREAD_JOIN)
		write(2, "pthread_join() didn't return 0\n", 32);
	else if (tab->exit_code == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (tab->exit_code == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (tab->exit_code == ERROR_SEM_WAIT)
		write(2, "sem_wait() returned -1\n", 24);
	else if (tab->exit_code == ERROR_SEM_POST)
		write(2, "sem_post() returned -1\n", 24);
	wrap_up(tab);
	return (1);
}

static int	grimreaper(t_tab *tab)
{
	int	i;

	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (tab->time_last_meal[i] != 0
			&& tab->time_last_meal[i] + tab->time_to_die < tab->current_time)
		{
			if (sem_wait(tab->print_sem) == -1)
				return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
			if (!tab->exit_code)
			{
				tab->exit_code = DEATH;
				printf("%lld %d "B_RED"died!"RESET"\n",
					(tab->current_time - tab->start_time), i + 1);
			}
			if (sem_post(tab->print_sem) == -1)
				return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
		}
	}
	return (1);
}

static int	update_current_time__grimreaper(t_tab *tab)
{
	while (!tab->exit_code)
	{
		tab->current_time = get_current_time();
		if (tab->current_time == -1)
			return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
		if (!grimreaper(tab))
			return (0);
		if (usleep(1000) == -1)
			return ((int)set_exit_code(tab, ERROR_USLEEP));
	}
	return (1);
}

static int	create_philosophers(t_tab *tab)
{
	int			i;

	tab->phi_n_c = 0;
	i = -1;
	tab->current_time = get_current_time();
	if (tab->current_time == -1)
		return ((int)set_exit_code(tab, ERROR_GETTIMEOFDAY));
	while (++i < tab->number_of_philosophers && !tab->exit_code)
		if (pthread_create(&tab->philosopher_thread, NULL, phi_f, tab) != 0)
			return ((int)set_exit_code(tab, ERROR_PTHREAD_CREATE));
	tab->pthreads_created = 1;
	if (tab->exit_code)
		return (0);
	return (1);
}

int	main(int ac, char **av)
{
	t_tab	tab;

	tab.exit_code = 0;
	tab.n_times_eaten = NULL;
	tab.pthreads_created = 0;
	tab.time_last_meal = NULL;
	if (ac < 5 || ac > 6)
	{
		set_exit_code(&tab, ERROR_AC);
		return (exit_error(&tab));
	}
	if (!initialize_variables(&tab, ac, av)
		|| !create_philosophers(&tab)
		|| !update_current_time__grimreaper(&tab))
		return (exit_error(&tab));
	if (!wrap_up(&tab))
		return (1);
	return (0);
}
