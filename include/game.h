/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:28:07 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 14:59:57 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_H
#define GAME_H

#define NB_PLAYERS_MAX 9
#define NB_TEAMS_MAX 9
#define MAP_NB_ROWS 10
#define MAP_NB_COLUMNS 10

#include <time.h>

#include "../include/shared_rcs.h"

struct position {
	int row;
	int col;
};
struct mapinfo {
	unsigned int nbp;
	unsigned char nbp_team[NB_TEAMS_MAX];
	unsigned int map[MAP_NB_ROWS][MAP_NB_COLUMNS];
};

struct player {
	unsigned int id;
	unsigned int team_id;
	unsigned int targ_id;
	struct position pos;
	time_t last_move;
};


int print_map(const struct mapinfo *m);
struct position find_player_pos(const struct mapinfo *m, unsigned int id);
int find_new_target(const struct shrcs *rcs, const struct mapinfo *m,
		const struct player *p);

#endif /* GAME_H */