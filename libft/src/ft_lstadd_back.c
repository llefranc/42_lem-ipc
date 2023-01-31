/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/18 18:05:53 by llefranc          #+#    #+#             */
/*   Updated: 2023/01/31 11:32:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"
void	ft_lstadd_back(t_list **alst, t_list *new)
{
	if (!alst)
		return ;
	if (!(*alst))
		*alst = new;
	else
	{
		while ((*alst)->next)
			alst = &(*alst)->next;
		(*alst)->next = new;
	}
}
