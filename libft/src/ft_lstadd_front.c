/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/18 17:35:50 by llefranc          #+#    #+#             */
/*   Updated: 2023/01/31 11:32:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"
void	ft_lstadd_front(t_list **alst, t_list *new)
{
	if (!alst || !new)
		return ;
	if (!(*alst))
		*alst = new;
	else
	{
		new->next = *alst;
		*alst = new;
	}
}
