/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/15 13:50:02 by llefranc          #+#    #+#             */
/*   Updated: 2023/01/31 11:32:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"
void	ft_putendl_fd(char *s, int fd)
{
	int		i;
	char	c;

	c = '\n';
	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
	write(fd, &c, 1);
}
