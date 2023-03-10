/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn_player.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:28:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/10 13:29:47 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPAWN_PLAYER_H
#define SPAWN_PLAYER_H

#include "game_utils.h"
#include "shared_rcs.h"

/**
 * unspawn_player() - Removes a player from the grid.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Sets the actual player position on the grid to 0, and decrements the number
 * of players in its team. This function is accessing shared ressources but not
 * protected by a semaphore lock, it needs then to be encapsulated with
 * semaphore loch/unlock operations.
*/
static inline void unspawn_player(struct mapinfo *m, const struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	m->game_state = E_STATE_PRINT;
	m->nbp_team[p->team_id - 1]--;
}

int spawn_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif /* SPAWN_PLAYER_H */