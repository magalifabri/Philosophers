/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfabri <mfabri@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 20:59:30 by mfabri            #+#    #+#             */
/*   Updated: 2021/04/16 07:28:44 by mfabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo_two.h"

void	*return_error(t_tab *tab, int error_num)
{
	tab->error_encountered = 1;
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (error_num == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_USLEEP)
		write(2, "usleep() returned -1\n", 22);
	else if (error_num == ERROR_PTHREAD_CREATE)
		write(2, "pthread_create() didn't return 0\n", 34);
	else if (error_num == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (error_num == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (error_num == ERROR_SEM_WAIT)
		write(2, "sem_wait() returned -1\n", 24);
	else if (error_num == ERROR_SEM_POST)
		write(2, "sem_post() returned -1\n", 24);
	free_malloced_variables(tab);
	sem_unlink("fork_availability");
	return (NULL);
}

int	check_if_all_are_sated(t_tab *tab)
{
	int	i;
	int	number_of_fat_philosophers;

	i = -1;
	number_of_fat_philosophers = 0;
	while (++i < tab->number_of_philosophers)
	{
		if (tab->n_times_eaten[i]
			== tab->number_of_times_each_philosopher_must_eat)
			number_of_fat_philosophers++;
		if (number_of_fat_philosophers == tab->number_of_philosophers)
		{
			write(1, B_GREEN"They're all fat. Good job!\n"RESET, 49);
			return (1);
		}
	}
	return (0);
}

/*
** Note(s) on monitor_philosophers():
** 
** Instead of using pthread_join() to make sure the main process doesn't exit
** before the threads are done, we trap the main process in a loop that it will
** only exit when the threads are done (a philosopher dies, all are fat or an
** error occurs).
** 
** This function supplies the value to the tab.current_time variable that is
** used by the threads. This is done so that they don't each have to do this
** individually.
*/

int	monitor_philosophers(t_tab *tab)
{
	while (1)
	{
		if (usleep(1000) == -1)
			return ((int)return_error(tab, ERROR_USLEEP));
		tab->current_time = get_current_time(tab);
		if (!tab->current_time)
			return (0);
		if (tab->error_encountered)
			return (0);
		if (tab->phi_died)
		{
			printf("\033[1;31mA philosopher has starved! Game over.\033[0m\n");
			// write(1, B_RED"A philosopher has starved! Game over.\n"RESET, 50);
			return (1);
		}
		if (check_if_all_are_sated(tab))
			return (1);
	}
	return (0);
}

/*
** Note(s) on create_philosophers():
** 
** Reason for usleep(): tab.phi_n needs to be copied over in each phi_f thread
** to tell the thread the number of the philosopher it represents. So we want
** to give each thread a bit of time to copy this value.
*/

int	create_philosophers(t_tab *tab)
{
	int	i;

	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		tab->current_time = get_current_time(tab);
		if (!tab->current_time)
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

int	main(int ac, char **av)
{
	t_tab	tab;

	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));
	if (!initialize_variables(&tab, ac, av))
		return (0);
	if (!create_philosophers(&tab))
		return (0);
	if (!monitor_philosophers(&tab))
		return (0);
	if (sem_unlink("fork_availability") == -1)
		return ((int)return_error(&tab, ERROR_SEM_UNLINK));
	free_malloced_variables(&tab);
	return (1);
}
