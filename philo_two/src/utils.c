#include "../philo_two.h"

int	put_status_msg(t_tab *tab, t_thread_var_struct *s, char *msg)
{
	// if (sem_wait(tab->put_status_msg_sem) == -1)
	// 	return ((int)return_error(tab, ERROR_SEM_WAIT));
	if (!tab->phi_died && !tab->error_encountered)
		printf("%lld %d %s\n",
			(tab->current_time - tab->start_time), s->phi_n + 1, msg);
	// if (sem_post(tab->put_status_msg_sem) == -1)
	// 	return ((int)return_error(tab, ERROR_SEM_POST));
	return (1);
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

void	free_malloced_variables(t_tab *tab)
{
	if (tab->n_times_eaten)
	{
		free(tab->n_times_eaten);
		tab->n_times_eaten = NULL;
	}
	if (tab->phi_t)
	{
		free(tab->phi_t);
		tab->phi_t = NULL;
	}
}
