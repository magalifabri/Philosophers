#include "../philo_three.h"

static void	*grimreaper(void *arg)
{
	t_tab	*tab;

	tab = (t_tab *)arg;
	while (1)
	{
		tab->current_time = get_current_time(tab);
		if (tab->current_time == -1)
			exit(EXIT_ERROR);
		if (tab->time_last_meal + tab->time_to_die < tab->current_time)
		{
			if (sem_wait(tab->print_sem) == -1)
				exit(EXIT_ERROR);
			tab->current_time = get_current_time(tab);
			if (tab->current_time == -1)
				exit(EXIT_ERROR);
			printf("%lld %d %sdied%s\n", (tab->current_time - tab->start_time),
				tab->phi_n + 1, B_RED, RESET);
			exit(EXIT_DEATH);
		}
		if (usleep(1000) == -1)
			exit(EXIT_ERROR);
	}
}

static void	eating(t_tab *tab)
{
	long long	time_done_eating;

	if (sem_wait(tab->fork_sem) == -1)
		exit(EXIT_ERROR);
	tab->current_time = get_current_time(tab);
	if (tab->current_time == -1)
		exit(EXIT_ERROR);
	time_done_eating = tab->current_time + tab->time_to_eat;
	put_status_msg(tab, "e");
	while (time_done_eating > tab->current_time)
		if (usleep(500) == -1)
			exit(EXIT_ERROR);
	if (sem_post(tab->fork_sem) == -1)
		exit(EXIT_ERROR);
	tab->times_eaten++;
	if (tab->number_of_times_each_philosopher_must_eat != -1
		&& tab->times_eaten == tab->number_of_times_each_philosopher_must_eat)
	{
		put_status_msg(tab, B_GREEN"is fat"RESET);
		exit(EXIT_EATEN_ENOUGH);
	}
}

static void	sleeping(t_tab *tab)
{
	long long	waking_time;

	put_status_msg(tab, "is sleeping");
	waking_time = tab->current_time + tab->time_to_sleep;
	while (waking_time > tab->current_time)
		if (usleep(500) == -1)
			exit(EXIT_ERROR);
	put_status_msg(tab, "is thinking");
}

void	phi_f(void *arg)
{
	t_tab		*tab;
	pthread_t	grimreaper_thread;

	tab = (t_tab *)arg;
	tab->time_last_meal = tab->start_time;
	if (pthread_create(&grimreaper_thread, NULL, grimreaper, tab) != 0)
		exit(EXIT_ERROR);
	while (1)
	{
		eating(tab);
		sleeping(tab);
	}
}
