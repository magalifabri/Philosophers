#include "../philo_three.h"

static int	wrap_up(t_tab *tab)
{
	int	ret;
	int	i;

	if (tab->phi_pid)
	{
		i = 0;
		while (i < tab->number_of_philosophers
			&& tab->phi_pid[i] != -1)
			kill(tab->phi_pid[i++], SIGKILL);
		free(tab->phi_pid);
	}
	ret = 1;
	if (tab->fork_sem_initialised)
	{
		if (sem_unlink("fork_sem") == -1)
			printf(B_RED"ERROR: "RESET"sem_unlink(fork_sem) returned -1\n");
		ret = 0;
	}
	if (tab->print_sem_initialised)
	{
		if (sem_unlink("print_sem") == -1)
			printf(B_RED"ERROR: "RESET"sem_unlink(print_sem) returned -1\n");
		ret = 0;
	}
	system("leaks philo_three > leaks_report.out");
	return (ret);
}

void	*return_error(t_tab *tab, int error_num)
{
	if (tab->print_sem_initialised)
		if (sem_wait(tab->print_sem) == -1)
			exit(EXIT_ERROR);
	write(2, B_RED"ERROR: "RESET, 19);
	if (error_num == ERROR_MUTEX)
		write(2, "pthread_mutex_(un)lock() returned -1\n", 38);
	else if (error_num == ERROR_GETTIMEOFDAY)
		write(2, "gettimeofday() returned -1\n", 28);
	else if (error_num == ERROR_MALLOC)
		write(2, "malloc() returned NULL\n", 24);
	else if (error_num == ERROR_BAD_ARGS)
		write(2, "bad arguments. Try again.\n", 27);
	else if (error_num == ERROR_FORK)
		write(2, "fork() returned < 0\n", 21);
	else if (error_num == ERROR_SEM_OPEN)
		write(2, "sem_open() failed\n", 19);
	else if (error_num == ERROR_SEM_UNLINK)
		write(2, "sem_unlink() returned -1\n", 26);
	else if (error_num == ERROR_AC)
		write(2, "too few or too many arguments\n", 31);
	else if (error_num == ERROR_CHILD)
		write(2, "something went wrong in child process\n", 31);
	wrap_up(tab);
	return (NULL);
}

/*
As soon as waitpid() returns with a pid, we know a philospopher has exited,
either because he's fat or because he's dead (or because an error occurred).
If he's fat, we let the other philosopher's continue; if he's dead or an
error occurred, we begin exiting.
*/

static int	monitor_philosophers(t_tab *tab)
{
	int		philosophers_left;
	int		exit_status;

	philosophers_left = tab->number_of_philosophers;
	while (philosophers_left--)
	{
		waitpid(-1, &exit_status, 0);
		if (WEXITSTATUS(exit_status) == EXIT_DEATH)
			return (1);
		if (WEXITSTATUS(exit_status) == EXIT_ERROR)
			return ((int)return_error(tab, ERROR_CHILD));
	}
	return (1);
}

static int	create_philosophers(t_tab *tab)
{
	int		i;
	pid_t	fork_ret;

	tab->current_time = get_current_time(tab);
	if (tab->current_time == -1)
		return ((int)return_error(tab, ERROR_GETTIMEOFDAY));
	i = -1;
	while (++i < tab->number_of_philosophers)
	{
		tab->phi_n = i;
		fork_ret = fork();
		if (fork_ret == 0)
			phi_f(tab);
		else
		{
			tab->phi_pid[i] = fork_ret;
			if (fork_ret == -1)
				return ((int)return_error(tab, ERROR_FORK));
		}
	}
	return (1);
}

int	main(int ac, char **av)
{
	t_tab	tab;

	pre_initialisation(&tab);
	if (!initialize_variables(&tab, ac, av))
		exit(EXIT_FAILURE);
	if (!create_philosophers(&tab))
		exit(EXIT_FAILURE);
	if (!monitor_philosophers(&tab))
		exit(EXIT_FAILURE);
	if (!wrap_up(&tab))
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
