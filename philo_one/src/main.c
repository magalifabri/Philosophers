#include "../philo_one.h"

void free_malloced_variables(t_tab *tab)
{
	write(1, CYAN"freeing allocated memory... "RESET, 40);
	if (tab->malloc_forks)
		free(tab->forks);
	if (tab->malloc_n_times_eaten)
		free(tab->n_times_eaten);
	if (tab->malloc_phi_t)
		free(tab->phi_t);
	write(1, CYAN"done.\n"RESET, 17);
}

int destroy_locks(t_tab *tab)
{
	int i;

	usleep(10000);
	write(1, CYAN"destroying mutex locks... "RESET, 38);
	i = -1;
	while (++i < tab->number_of_philosophers)
		if (pthread_mutex_destroy(&tab->forks[i].lock) != 0)
			return ((int)return_error(tab, ERROR_MUTEX_DESTROY));
	write(1, CYAN"done.\n"RESET, 18);
	return (1);
}

void *return_error(t_tab *tab, int error_num)
{
	tab->error_encountered = 1;
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX_LOCK)
		write(2, "pthread_mutex_lock() returned -1\n", 34);
	if (error_num == ERROR_MUTEX_UNLOCK)
		write(2, "pthread_mutex_unlock() returned -1\n", 36);
	if (error_num == ERROR_MUTEX_INIT)
		write(2, "pthread_mutex_init() didn't return 0\n", 38);
	if (error_num == ERROR_MUTEX_DESTROY)
		write(2, "pthread_mutex_destroy() didn't return 0\n", 41);
	if (error_num == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	free_malloced_variables(tab);
	return (NULL);
}

int check_if_all_have_returned(t_tab *tab)
{
	int i;
	int returned;

	i = -1;
	returned = 0;
	while (++i < tab->number_of_philosophers)
	{
		if (tab->n_times_eaten[i]
		== tab->number_of_times_each_philosopher_must_eat)
			returned++;
		if (returned == tab->number_of_philosophers)
		{
			write(1, B_GREEN"They're all fat. Welcome to America!\n"RESET, 49);
			return (1);
		}
	}
	return (0);
}

int monitor_philosophers(t_tab *tab)
{
	while (1)
	{
		if (usleep(1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
		if (!(tab->current_time = get_current_time(tab)))
			return (0);
		if (tab->error_encountered)
			return (0);
		if (tab->phi_died)
		{
			write(1, B_RED"A philosopher has starved! Game over.\n"RESET, 50);
			return (1);
		}
		if (check_if_all_have_returned(tab))
			return (1);
	}
	return (0);
}

/*
Reason for usleep(): tab.phi_n needs to be copied over in each phi_f thread, so we can only create threads as quickly as phi_f can copy
*/

int create_philosophers(t_tab *tab)
{
	int i;

	if (!(tab->phi_t
	= malloc(sizeof(pthread_t) * tab->number_of_philosophers + 1)))
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->malloc_phi_t = 1;
	tab->phi_t[tab->number_of_philosophers] = NULL;
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (!(tab->current_time = get_current_time(tab)))
			return (0);
		tab->phi_n = i;
		if (pthread_create(&tab->phi_t[i], NULL, phi_f, tab) != 0)
			return ((int)return_error(tab, ERROR_PTHREAD_CREATE));
		pthread_detach(tab->phi_t[i]);
		if (usleep(100) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
	}
	return (1);
}

/*
Each fork is protected for a mutex lock. The mutex is locked only while the availability of the fork is checked and changed (if the fork is indeed available). While the fork is being used, the mutex isn't locked, so that other threads can check on the availability of the fork and continue on their way if it's not, instead of having to wait right there for it to actually become available again.
Keeping the mutex locked while the fork is in use should result in much less locking and unlocking and with that, shorter and better readable code. So I'll probably do it like that in the future.
*/

int main(int ac, char **av)
{
	t_tab tab;
	int i;

	initialize_malloc_indicators(&tab);
	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));
	if (!initialize_variables_and_locks(&tab, ac, av))
		return (0);
	i = -1;
	while (++i < tab.number_of_philosophers)
		tab.n_times_eaten[i] = 0;
	if (!create_philosophers(&tab))
		return (0);
	if (!monitor_philosophers(&tab))
		return (0);
	if (!destroy_locks(&tab))
		return (0);
	free_malloced_variables(&tab);
	return (1);
}