/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_move.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:51:26 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 19:54:53 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLAYER_MOVE_H
#define PLAYER_MOVE_H

#include "shared_rcs.h"
#include "game_utils.h"

int is_player_dead(struct mapinfo *m, const struct player *p);
int player_move(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif /* PLAYER_MOVE_H */