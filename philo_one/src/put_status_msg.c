/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_status_msg.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfabri <mfabri@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/15 07:30:49 by mfabri            #+#    #+#             */
/*   Updated: 2021/04/15 19:05:32 by mfabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo_one.h"

static int	get_len(char *s)
{
	int	len;

	len = 0;
	while (*s++)
		len++;
	return (len);
}

static char	*copy_strings(int total_len, char **arg_ptrs, int n, va_list args)
{
	char	*cat_string;
	int		i;
	int		i2;
	int		i3;

	cat_string = malloc(total_len + 1);
	if (!cat_string)
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

static char	*concatenate_strings(int num, ...)
{
	va_list	args;
	int		i;
	char	**arg_ptrs;
	int		total_len;

	arg_ptrs = malloc(sizeof(char *) * num);
	if (!arg_ptrs)
		return (NULL);
	va_start(args, num);
	total_len = 0;
	i = -1;
	while (++i < num)
	{
		arg_ptrs[i] = va_arg(args, char *);
		total_len += get_len(arg_ptrs[i]);
	}
	return (copy_strings(total_len, arg_ptrs, num, args));
}

char	*ft_uitoa(unsigned int n)
{
	int				len;
	char			*s;
	unsigned int	n_cpy;

	n_cpy = n;
	len = 0;
	if (n == 0)
		len = 1;
	while (n > 0)
	{
		len++;
		n /= 10;
	}
	s = malloc(len + 1);
	if (!s)
		return (NULL);
	s[len--] = '\0';
	while (n_cpy > 0 || (!n_cpy && !len))
	{
		s[len--] = (n_cpy % 10) + 48;
		n_cpy /= 10;
	}
	return (s);
}

int	put_status_msg(t_tab *tab, long long time, int phi_n, char *message)
{
	char	*c_time;
	char	*c_phi_n;
	char	*concat;

	c_time = ft_uitoa((int)time);
	if (!c_time)
		return (0);
	c_phi_n = ft_uitoa(phi_n);
	if (!c_phi_n)
	{
		free(c_time);
		return (0);
	}
	concat = concatenate_strings(5, c_time, " ", c_phi_n, " ", message);
	free(c_time);
	free(c_phi_n);
	if (!concat)
		return (0);
	if (!tab->phi_died && !tab->error_encountered)
		write(1, concat, get_len(concat));
	free(concat);
	return (1);
}
