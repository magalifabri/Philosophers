/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfabri <mfabri@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/21 10:38:35 by mfabri            #+#    #+#             */
/*   Updated: 2021/04/21 10:38:36 by mfabri           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo_one.h"

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
