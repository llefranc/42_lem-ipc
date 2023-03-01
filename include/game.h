/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:28:07 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 16:41:01 by llefranc         ###   ########.fr       */
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

/**
 * struct position - Represents a square on a grid.
 * @row: Row number of the map.
 * @col: Column number of the map.
*/
struct position {
	int row;
	int col;
};

/**
 * struct mapinfo - Contains all the map information.
 * @nbp: The number of player which joined the game since it started.
 * @nbp_team: The number of players per team actually playing.
 * @map: The game map containing all the players.
 *
 * All fields of mapinfo are stored in the shared memory segment, which allows
 * each process to be able to see the actual game state and interact with it.
*/
struct mapinfo {
	unsigned int nbp;
	unsigned char nbp_team[NB_TEAMS_MAX];
	unsigned int map[MAP_NB_ROWS][MAP_NB_COLUMNS];
};

/**
 * struct player - Contains the player information.
 * @id: The id of the player resulting in the combination of its personnal id,
 *      which is the number of player who previously joined when he joined the
 *      game, and its team id (3 bytes for player id, 1 byte for team id).
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 * @targ_id: The id of an ennemy player that the player is actually targeting.
 * @pos: The player position on the grid.
 * @last_move: The time of when its last moved occured.
*/
struct player {
	unsigned int id;
	unsigned int team_id;
	unsigned int targ_id;
	struct position pos;
	time_t last_move;
};


void print_map(const struct mapinfo *m);
struct position find_player_pos(const struct mapinfo *m, unsigned int id);
int find_new_target(const struct shrcs *rcs, const struct mapinfo *m,
		const struct player *p);

#endif /* GAME_H */