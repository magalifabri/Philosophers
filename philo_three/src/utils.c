#include "../philo_three.h"

static int	ft_isspace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f'
		|| c == '\r');
}

int	ft_atoi(const char *str)
{
	int					i;
	int					neg;
	unsigned long int	res;

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

long long	get_current_time(t_tab *tab)
{
	struct timeval	tp;
	long long		passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return ((long long)return_error(tab, ERROR_GETTIMEOFDAY));
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}

int	wrap_up(t_tab *tab)
{
	int	ret;

	ret = 1;
	if (tab->phi_pid)
		free(tab->phi_pid);
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
	return (ret);
}
