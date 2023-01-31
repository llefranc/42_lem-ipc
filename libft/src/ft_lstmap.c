/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/20 12:53:18 by llefranc          #+#    #+#             */
/*   Updated: 2023/01/31 11:32:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"
t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*first;
	t_list	*newlst;
	t_list	*temp;

	first = NULL;
	newlst = NULL;
	temp = NULL;
	if (!del)
		return (NULL);
	if (!(first = ft_lstnew((*f)(lst->content))))
		return (NULL);
	temp = first;
	lst = lst->next;
	while (lst && f)
	{
		if (!(newlst = ft_lstnew((*f)(lst->content))))
		{
			ft_lstclear(&first, del);
			return (NULL);
		}
		temp->next = newlst;
		temp = newlst;
		lst = lst->next;
	}
	return (first);
}
