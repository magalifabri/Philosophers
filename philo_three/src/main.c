#include "../philo_three.h"

/*
** Note(s) on grimreaper():
** 
** Philosophers may die of starvation while standing in the "dining room queue"
** (semaphore) and it's important to report on their unfortunate demise within
** 10ms of its occurrance (and halt the program immediately afterwards?). But
** while they are waiting, they are incapable of doing anything else, and the
** other philosophers and the parent process also aren't aware of whether or
** not the philosopher has actually starved. Therefore a pthread is created
** within the child process to keep an eye on things and quit the process if
** the philosopher doesn't manage to eat in time.
*/

void *grimreaper(void *arg)
{
	t_tab *tab;
	tab = (t_tab *)arg;

	while (1)
	{
		if (usleep(1000) == -1)
			exit(EXIT_ERROR);
		if ((tab->current_time = get_current_time(tab)) == -1)
			exit(EXIT_ERROR);
		if (tab->time_last_meal + tab->time_to_die <= tab->current_time)
		{
			if (!put_status_msg((tab->current_time - tab->start_time)
			, tab->phi_n + 1, B_RED"died\n"RESET))
				exit(EXIT_ERROR);
			exit(EXIT_DEATH);
		}
	}
}

void waiting_and_eating(t_tab *tab)
{
	if (sem_wait(tab->fork_availability) == -1)
		exit(EXIT_ERROR);
	if (!put_status_msg((tab->current_time - tab->start_time)
	, tab->phi_n + 1, "is eating\n"))
		exit(EXIT_ERROR);
	tab->time_last_meal = tab->current_time;
	if (usleep(tab->time_to_eat * 1000) == -1)
		exit(EXIT_ERROR);
	tab->times_eaten++;
	if (tab->number_of_times_each_philosopher_must_eat != -1 &&
	tab->times_eaten >= tab->number_of_times_each_philosopher_must_eat)
	{
		if (!(put_status_msg((tab->current_time - tab->start_time),
		tab->phi_n + 1, B_GREEN"is fat enough\n"RESET)))
			exit(EXIT_ERROR);
		if (sem_post(tab->fork_availability) == -1)
			exit(EXIT_ERROR);
		exit(EXIT_EATEN_ENOUGH);
	}
}

void phi_f(void *arg)
{
	t_tab *tab;
	pthread_t grimreaper_thread;
	
	tab = (t_tab *)arg;
	if ((tab->current_time = get_current_time(tab)) == -1)
		exit(EXIT_ERROR);
	tab->time_last_meal = tab->current_time;
	if (pthread_create(&grimreaper_thread, NULL, grimreaper, tab) != 0)
		exit(EXIT_ERROR);
	while(1)
	{
		waiting_and_eating(tab);
		if (!put_status_msg((tab->current_time - tab->start_time)
		, tab->phi_n + 1, "is sleeping\n"))
			exit(EXIT_ERROR);
		if (sem_post(tab->fork_availability) == -1)
			exit(EXIT_ERROR);
		if (usleep(tab->time_to_sleep * 1000) == -1)
			exit(EXIT_ERROR);
		if (!put_status_msg((tab->current_time - tab->start_time)
		, tab->phi_n + 1, "is thinking\n"))
			exit(EXIT_ERROR);
	}
}

/*
** Note(s) on monitor_child_processes():
** 
** As soon as wait() returns with a pid, we know a philospopher has exited,
** either because he's fat or because he's dead (or because an error occurred).
** If he's fat, we let the other philosopher's continue; if he's dead (or an
** error occurred), we kill the other philosophers.
*/

int monitor_child_processes(t_tab *tab)
{
	int philosophers_left;
	int exit_status;
	pid_t pid;

	philosophers_left = tab->number_of_philosophers;
	while (philosophers_left--)
	{
		pid = wait(&exit_status);
		if (WEXITSTATUS(exit_status) == EXIT_DEATH
		|| WEXITSTATUS(exit_status) == EXIT_ERROR)
		{
			if (WEXITSTATUS(exit_status) == EXIT_DEATH)
				write(1, B_RED"Well.. that's a bit unfortunate..\n"RESET, 46);
			else if (WEXITSTATUS(exit_status) == EXIT_ERROR)
				return ((int)return_error(tab, ERROR_CHILD));
			while (tab->number_of_philosophers--)
				kill(tab->phi_pid[tab->number_of_philosophers], SIGKILL);
			break;
		}
	}
	if (philosophers_left == -1)
		write(1, B_GREEN"Good job! They're all fat.\n"RESET, 39);
	return (1);
}

int main(int ac, char **av)
{
	t_tab tab;
	int i;

	tab.malloc_phi_pid = 0;
	if (ac < 5 || ac > 6)
		return ((int)return_error(&tab, ERROR_AC));
	if (!initialize_variables(&tab, ac, av))
		return (0);
	i = -1;
	while (++i < tab.number_of_philosophers)
	{
		tab.phi_n = i;
		pid_t fork_ret = fork();
		if (fork_ret == 0)
			phi_f(&tab);
		else if (fork_ret > 0)
			tab.phi_pid[i] = fork_ret;
		else
			return ((int)return_error(&tab, ERROR_FORK));
	}
	monitor_child_processes(&tab);
	if (sem_unlink("fork_availability") == -1)
		return ((int)return_error(&tab, ERROR_SEM_UNLINK));
	free_malloced_variables(&tab);
	return (1);
}
