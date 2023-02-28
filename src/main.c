/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/28 14:28:04 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shared_rcs.h"
#include "../include/game.h"

#include <sys/ipc.h>

int main(int ac, char **av)
{
	(void)ac;
	(void)av;
	key_t key;
	struct shrcs rcs = {};

	if ((key = keygen(1)) == -1)
		return 1;
	if (get_shared_rcs(&rcs, key, sizeof(struct mapinfo)) == -1)
		return 1;
	if (clean_shared_rcs(&rcs, E_CLEAN_ALL) < 0)
		return 1;
	return 0;
}