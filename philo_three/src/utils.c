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

static void	eat_or_die(t_tab *tab, long long timestamp)
{
	if (tab->time_last_meal + tab->time_to_die < tab->current_time)
	{
		printf("%lld %d "B_RED"died"RESET"\n",
			(tab->current_time - tab->start_time), tab->phi_n + 1);
		exit(EXIT_DEATH);
	}
	else
	{
		tab->time_last_meal = tab->current_time;
		printf("%lld %d has taken a fork\n%lld %d has taken a fork\n",
			timestamp, tab->phi_n + 1, timestamp, tab->phi_n + 1);
		printf("%lld %d is eating\n", timestamp, tab->phi_n + 1);
	}
}

int	put_status_msg(t_tab *tab, char *msg)
{
	int			ret;
	long long	timestamp;

	ret = 1;
	if (sem_wait(tab->print_sem) == -1)
		exit(EXIT_ERROR);
	tab->current_time = get_current_time(tab);
	if (tab->current_time == -1)
		exit(EXIT_ERROR);
	timestamp = tab->current_time - tab->start_time;
	if (msg[0] == 'e')
		eat_or_die(tab, timestamp);
	else
		printf("%lld %d %s\n", timestamp, tab->phi_n + 1, msg);
	if (sem_post(tab->print_sem) == -1)
		exit(EXIT_ERROR);
	return (ret);
}
