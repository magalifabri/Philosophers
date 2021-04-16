/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize_variables.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfabri <mfabri@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 07:30:39 by mfabri            #+#    #+#             */
/*   Updated: 2021/04/16 14:06:42 by mfabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo_one.h"

void	initialize_malloc_and_mutex_indicators(t_tab *tab)
{
	tab->forks = NULL;
	tab->n_times_eaten = NULL;
	tab->phi_t = NULL;
	tab->mutexes_initialized = 0;
}

static int	initialize_more(t_tab *tab)
{
	int	i;

	tab->forks = malloc(sizeof(t_frk) * tab->number_of_philosophers);
	if (!tab->forks)
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->n_times_eaten = malloc(sizeof(int) * tab->number_of_philosophers);
	if (!tab->n_times_eaten)
		return ((int)return_error(tab, ERROR_MALLOC));
	tab->phi_t = malloc(sizeof(pthread_t) * tab->number_of_philosophers);
	if (!tab->phi_t)
		return ((int)return_error(tab, ERROR_MALLOC));
	i = -1;
	while (++i < tab->number_of_philosophers)
		tab->n_times_eaten[i] = 0;
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		if (pthread_mutex_init(&tab->forks[i].lock, NULL) != 0)
			return ((int)return_error(tab, ERROR_MUTEX_INIT));
		tab->forks[i].available = 1;
	}
	tab->mutexes_initialized = 1;
	return (1);
}

/*
** Note(s) on initialize_variables_and_locks():
** 
** "tab->number_of_times_each_philosopher_must_eat = -1": -1 is a sentinel
** value for an absence of value (a value for it wasn't supplied with av)
*/

int	initialize_variables_and_locks(t_tab *tab, int ac, char **av)
{
	tab->number_of_philosophers = ft_atoi(av[1]);
	tab->time_to_die = ft_atoi(av[2]);
	tab->time_to_eat = ft_atoi(av[3]);
	tab->time_to_sleep = ft_atoi(av[4]);
	if (tab->number_of_philosophers < 2 || tab->time_to_die < 1
		|| tab->time_to_eat < 1 || tab->time_to_sleep < 1)
		return ((int)return_error(tab, ERROR_BAD_ARGS));
	if (ac == 5)
		tab->number_of_times_each_philosopher_must_eat = -1;
	else
	{
		tab->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
		if (!tab->number_of_times_each_philosopher_must_eat)
			return ((int)return_error(tab, ERROR_BAD_ARGS));
	}
	tab->phi_died = 0;
	tab->error_encountered = 0;
	tab->start_time = get_current_time(tab);
	if (!tab->start_time)
		return (0);
	return (initialize_more(tab));
}

void	initialize_variables_phi_f(t_tab *tab, t_thread_var_struct *s)
{
	s->phi_n = tab->phi_n;
	s->left_fork_held = 0;
	s->right_fork_held = 0;
	s->phi_state = 't';
	s->time_sleep_start = tab->current_time;
	s->time_last_meal = tab->current_time;
}
