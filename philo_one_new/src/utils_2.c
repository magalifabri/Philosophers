#include "../philo_two.h"

long long	get_current_time(void)
{
	struct timeval	tp;
	long long		passed_time;

	if (gettimeofday(&tp, 0) == -1)
		return (-1);
	passed_time = tp.tv_sec;
	passed_time *= 1000;
	passed_time += (tp.tv_usec / 1000);
	return (passed_time);
}

/*
A little helper function that unlocks two mutex locks and returns 0.
Created to trim down the length of eating().
*/

int	mutex_unlock__return_0(t_tab *tab, pthread_mutex_t *lock_1,
	pthread_mutex_t *lock_2, int return_value)
{
	if (lock_1)
		if (pthread_mutex_unlock(lock_1) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	if (lock_2)
		if (pthread_mutex_unlock(lock_2) == -1)
			return ((int)set_exit_code(tab, ERROR_MUTEX_UNLOCK));
	return (return_value);
}

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
