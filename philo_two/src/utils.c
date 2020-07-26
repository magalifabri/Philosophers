#include "../philo_two.h"

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

void free_malloced_variables(t_tab *tab)
{
	write(1, CYAN"freeing allocated memory... "RESET, 40);
	if (tab->malloc_n_times_eaten)
		free(tab->n_times_eaten);
	if (tab->malloc_phi_t)
		free(tab->phi_t);
	write(1, CYAN"done.\n"RESET, 17);
}
