#include "../philo_one.h"

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
