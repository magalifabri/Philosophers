#include "../philo_one.h"

void	*grimreaper(void *arg)
{
	t_thread_var_struct	*s;

	s = (t_thread_var_struct *)arg;
	while (s->time_last_meal + s->tab->time_to_die > s->tab->current_time)
	{
		if (usleep(1000) == -1)
			return (set_exit_code(s->tab, ERROR_USLEEP));
		if (s->tab->exit_code)
			return (NULL);
	}
	check_vitality(s->tab, s);
	return (NULL);
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
