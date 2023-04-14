/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_spawn.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:28:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 19:56:39 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLAYER_SPAWN_H
#define PLAYER_SPAWN_H

#include "game_utils.h"
#include "shared_rcs.h"

/**
 * player_unspawn() - Removes a player from the grid.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Sets the actual player position on the grid to 0, and decrements the number
 * of players in its team. This function is accessing shared ressources but not
 * protected by a semaphore lock, it needs then to be encapsulated with
 * semaphore loch/unlock operations.
*/
static inline void player_unspawn(struct mapinfo *m, const struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	m->game_state = E_STATE_PRINT;
	m->nbp_team[p->team_id - 1]--;
}

int player_spawn(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif /* PLAYER_SPAWN_H */