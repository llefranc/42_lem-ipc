/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_player.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:51:26 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/10 12:22:42 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MOVE_PLAYER_H
#define MOVE_PLAYER_H

#include "shared_rcs.h"
#include "game_utils.h"

int is_dead(const struct shrcs *rcs, struct mapinfo *m, const struct player *p);
int move_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif /* MOVE_PLAYER_H */