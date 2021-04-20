/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   phi_f_eating_to_thinking.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfabri <mfabri@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 07:30:42 by mfabri            #+#    #+#             */
/*   Updated: 2021/04/20 09:06:19 by mfabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo_one.h"

static int	lay_down_forks(t_tab *tab, t_thread_var_struct *s)
{
	if (pthread_mutex_lock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_LOCK));
	tab->forks[s->phi_n].available = 1;
	if (pthread_mutex_unlock(&tab->forks[s->phi_n].lock) == -1)
		return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	if (s->phi_n == 0)
	{
		if (pthread_mutex_lock(
				&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_LOCK));
		tab->forks[tab->number_of_philosophers - 1].available = 1;
		if (pthread_mutex_unlock(
				&tab->forks[tab->number_of_philosophers - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	}
	else
	{
		if (pthread_mutex_lock(&tab->forks[s->phi_n - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_LOCK));
		tab->forks[s->phi_n - 1].available = 1;
		if (pthread_mutex_unlock(&tab->forks[s->phi_n - 1].lock) == -1)
			return ((int)return_error(tab, ERROR_MUTEX_UNLOCK));
	}
	return (1);
}

static int	finish_eating_and_obesity_check(t_tab *tab, t_thread_var_struct *s)
{
	if (!lay_down_forks(tab, s))
		return (0);
	s->right_fork_held = 0;
	s->left_fork_held = 0;
	if (!put_status(tab, s->phi_n + 1, "put his forks down"))
		return (0);
	tab->n_times_eaten[s->phi_n]++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->n_times_eaten[s->phi_n]
		>= tab->number_of_times_each_philosopher_must_eat)
	{
		
		if (!put_status(tab, s->phi_n + 1, B_GREEN"is fat"RESET))
			return (0);
	}
	return (1);
}

int	eating_to_thinking(t_tab *tab, t_thread_var_struct *s)
{
	if (!finish_eating_and_obesity_check(tab, s))
		return (0);
	if (!put_status(tab, s->phi_n + 1, "is sleeping"))
		return (0);
	if (usleep(tab->time_to_sleep * 1000) == -1)
		return ((int)return_error(tab, ERROR_USLEEP));
	if (!put_status(tab, s->phi_n + 1, "is thinking"))
		return (0);
	return (1);
}
