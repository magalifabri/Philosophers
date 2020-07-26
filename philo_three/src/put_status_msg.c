#include "../philo_three.h"

static int get_len(char *s)
{
	int len;

	len = 0;
	while (*s++)
		len++;
	return (len);
}

static char *copy_strings(int total_len, char **arg_ptrs, int n, va_list args)
{
	char *cat_string;
	int i;
	int i2;
	int i3;

	if (!(cat_string = malloc(total_len + 1)))
		return (NULL);
	i = -1;
	i3 = -1;
	while (++i < n)
	{
		i2 = -1;
		while (arg_ptrs[i][++i2])
			cat_string[++i3] = arg_ptrs[i][i2];
	}
	cat_string[++i3] = '\0';
	free(arg_ptrs);
	va_end(args);
	return (cat_string);
}

static char *concatenate_strings(int num, ...)
{
	va_list args;
	int i;
	char **arg_ptrs;
	int *arg_lengths;
	int total_len;

	if (!(arg_ptrs = malloc(sizeof(char *) * num)))
		return (NULL);
	if (!(arg_lengths = malloc(sizeof(int) * num)))
	{
		free(arg_ptrs);
		return (NULL);
	}
	va_start(args, num);
	total_len = 0;
	i = -1;
	while (++i < num)
	{
		arg_ptrs[i] = va_arg(args, char *);
		arg_lengths[i] = get_len(arg_ptrs[i]);
		total_len += arg_lengths[i];
	}
	free(arg_lengths);
	return (copy_strings(total_len, arg_ptrs, num, args));
}

char	*ft_itoa(int n)
{
	int		neg;
	char	*str;
	int		len;
	long	n_cpy;

	n_cpy = n;
	neg = (n < 0) ? (1) : (0);
	n_cpy = (n_cpy < 0) ? (-n_cpy) : (n_cpy);
	len = 0;
	(n <= 0) && (len++);
	while (n)
		n = len++ ? n / 10 : n / 10;
	if (!(str = malloc(sizeof(char) * (len + 1))))
		return (NULL);
	str[len--] = '\0';
	(!n_cpy) && (str[len--] = '0');
	while (n_cpy)
	{
		str[len--] = (n_cpy % 10) + '0';
		n_cpy /= 10;
	}
	(neg == 1) && (str[len--] = '-');
	return (str);
}

int put_status_msg(long long time, int phi_n, char *message)
{
	char *c_time;
	char *c_phi_n;
	char *concat;

	if (!(c_time = ft_itoa((int)time)))
		return (0);
	if (!(c_phi_n = ft_itoa(phi_n)))
	{
		free(c_time);
		return (0);
	}
	concat = concatenate_strings(5, c_time, " ", c_phi_n, " ", message);
	free(c_time);
	free(c_phi_n);
	if (!concat)
		return (0);
	write(1, concat, get_len(concat));
	free(concat);
	return (1);
}