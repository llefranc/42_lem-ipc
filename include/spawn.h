/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:28:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 15:30:01 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPAWN_H
#define SPAWM_H

#include "game.h"
#include "shared_rcs.h"

int spawn_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p);

#endif