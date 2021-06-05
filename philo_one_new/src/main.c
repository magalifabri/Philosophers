#include "../philo_two.h"

int	exit_error(t_tab *tab)
{
	write(2, B_RED"ERROR: "RESET, 19);
	if (tab->exit_code == ERROR_MUTEX_LOCK)
		write(2, "pthread_mutex_lock() returned -1\n", 34);
	else if (tab->exit_code == ERROR_MUTEX_UNLOCK)
		write(2, "pthread_mutex_unlock() returned -1\n", 36);
	else if (tab->exit_code == ERROR_MUTEX_INIT)
		write(2, "pthread_mutex_init() didn't return 0\n", 38);
	else if (tab->exit_code == ERROR_MUTEX_DESTROY)
		write(2, "pthread_mutex_destroy() didn't return 0\n", 41);
	else if (tab->exit_code == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (tab->exit_code == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (tab->exit_code == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (tab->exit_code == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (tab->exit_code == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (tab->exit_code == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (tab->exit_code == ERROR_PTHREAD_JOIN)
		write(2, "pthread_join() didn't return 0\n", 32);
	wrap_up(tab);
	return (1);
}

void	*set_exit_code(t_tab *tab, int exit_code)
{
	if (tab->exit_code == 0
		|| tab->exit_code == DEATH
		|| tab->exit_code == ALL_FAT)
		tab->exit_code = exit_code;
	return (NULL);
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
			// if (sem_wait(tab->print_sem) == -1)
			// 	return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
			if (pthread_mutex_lock(&tab->print_lock) == -1)
				return ((int)set_exit_code(tab, ERROR_MUTEX_LOCK));
			if (!tab->exit_code)
			{
				tab->exit_code = DEATH;
				printf("%lld %d "B_RED"died!"RESET"\n",
					(tab->current_time - tab->start_time), i + 1);
			}
			// if (sem_post(tab->print_sem) == -1)
			// 	return ((int)set_exit_code(tab, ERROR_SEM_WAIT));
			if (pthread_mutex_unlock(&tab->print_lock) == -1)
				return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
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

	// tab.exit_code = 0;
	// tab.n_times_eaten = NULL;
	// tab.pthreads_created = 0;
	// tab.time_last_meal = NULL;
	pre_initialisation(&tab);
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
