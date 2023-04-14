/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_lobby.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 19:35:29 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 19:40:55 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_LOBBY_H
#define GAME_LOBBY_H

#include "shared_rcs.h"
#include "game_utils.h"

int join_game(const struct shrcs *rcs, struct mapinfo *m, _Bool is_graphic_mode);
int wait_for_players(const struct shrcs *rcs, const struct mapinfo *m);

#endif /* GAME_LOBBY_H */