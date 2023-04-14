/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_mode.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 19:57:58 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 20:03:23 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_MODE_H
#define GAME_MODE_H

#include "shared_rcs.h"
#include "game_utils.h"

int graphic_mode(const struct shrcs *rcs, struct mapinfo *m);
int player_mode(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif /* GAME_MODE_H */