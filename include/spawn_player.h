/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn_player.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:28:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/03 16:17:01 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPAWN_PLAYER_H
#define SPAWN_PLAYER_H

#include "game_utils.h"
#include "shared_rcs.h"

int spawn_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p);
int unspawn_player(const struct shrcs *rcs, struct mapinfo *m, const struct
		player *p);

#endif /* SPAWN_PLAYER_H */