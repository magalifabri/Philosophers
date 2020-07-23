#include "../philo_three.h"

static int ft_isspace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f'
	|| c == '\r') ? (1) : (0);
}

int ft_atoi(const char *str)
{
	int i;
	int neg;
	unsigned long int res;

	i = 0;
	while (str[i] && ft_isspace(str[i]))
		i++;
	neg = 1;
	if (str[i] == '-' || str[i] == '+')
	{
		(str[i] == '-') && (neg = -neg);
		i++;
	}
	res = 0;
	while (str[i] == '0')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i++] - '0');
		if (res > 9223372036854775807 && neg == 1)
			return (-1);
		if (res - 1 > 9223372036854775807 && neg == -1)
			return (0);
	}
	return (res * neg);
}

void free_malloced_variables(t_tab *tab)
{
	if (tab->malloc_phi_pid)
		free(tab->phi_pid);
}

void *return_error(t_tab *tab, int error_num)
{
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
	free_malloced_variables(tab);
	sem_unlink("fork_availability");
	return (NULL);
}

long long get_current_time(t_tab *tab)
{
	struct timeval tp;
	long long passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)return_error(tab, ERROR_GETTIMEOFDAY));
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}
