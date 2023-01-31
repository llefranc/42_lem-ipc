/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/20 11:57:32 by llefranc          #+#    #+#             */
/*   Updated: 2023/01/31 11:32:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"
void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*temp;

	temp = NULL;
	if (lst && del)
	{
		while (*lst)
		{
			(*del)((*lst)->content);
			temp = (*lst)->next;
			(*lst)->next = NULL;
			free(*lst);
			*lst = temp;
		}
	}
}
