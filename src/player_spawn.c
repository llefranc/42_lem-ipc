/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_spawn.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:27:54 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 20:10:05 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/player_spawn.h"

#include "../include/log.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * is_valid_spawn() - Checks if a player can spawn on a square from the grid.
 * @m: Contains all the map information.
 * @spawn_r: Row number of the map.
 * @spawn_c: Col number of the map.
 *
 * To be a valid spawn, the square must be empty and surrounded by no other
 * player. Ex (where x is the empty square to spawn):
 *     0 0 0
 *     0 x 0
 *     0 0 0
 *
 * Return: 1 if the spawn is valid, 0 otherwise.
*/
static _Bool is_valid_spawn(const struct mapinfo *m, int spawn_r, int spawn_c)
{
	if (!(spawn_r >= 0 && spawn_r < MAP_NB_ROWS) ||
	    !(spawn_c >= 0 && spawn_c < MAP_NB_COLS))
		return 0;

	for (int row = spawn_r - 1; row <= spawn_r + 1; ++row) {
		if (!(row >= 0 && row < MAP_NB_ROWS))
			continue;
		for (int col = spawn_c - 1; col <= spawn_c + 1; ++col) {
			if (!(col >= 0 && col < MAP_NB_COLS))
				continue;
			if (get_id(m, row, col) != 0)
				return 0;
		}
	}
	return 1;
}

/**
 * find_spawn_pos() - Finds a valid spawn position.
 * @m: Contains all the map information.
 *
 * Tries to find a valid (i.e not surrounded by any other player) random
 * spawn x times (x = MAP_NB_ROWS * MAP_NB_COLS). If no random spawn is found
 * after x times, then iterates through the map and checks all squares.
 *
 * Return: A valid position if a spawn was found, a position of (-1,-1)
 * 	   otherwise.
*/
static struct position find_spawn_pos(struct mapinfo *m)
{
	int row;
	int col;
	int nb_try = MAP_NB_ROWS * MAP_NB_COLS;
	struct position pos = { .row = -1, .col = -1 };

	do {
		row = rand() % MAP_NB_ROWS;
		col = rand() % MAP_NB_COLS;
		if (is_valid_spawn(m, row, col)) {
			set_pos(&pos, row, col);
			return pos;
		}
	} while (--nb_try);

	for (int row = 0; row < MAP_NB_ROWS; ++row) {
		for (int col = 0; col < MAP_NB_COLS; ++col) {
			if (is_valid_spawn(m, row, col)) {
				set_pos(&pos, row, col);
				return pos;
			}
		}
	}
	return pos;
}

static inline void spawn_update_map(struct mapinfo *m, const struct player *p)
{
	m->nbp++;
	m->nbp_team[p->team_id - 1]++;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

/**
 * player_spawn() - Spawns a player on the grid.
 * @rcs: Contains all information for shared ressources.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Spawns a player on the grid if there is a square available (i.e. with no
 * other player at left, right, up and down position). For odd team, it will
 * start to look for an available square from the top of the map; for pair team
 * it will start to look from the bottom. If there is a position available then:
 *     - Look for a potential ennemy to target
 *     - Updates the map information
 *     - Updates the player information
 *
 * Return: 0 on success, -1 if an error occured.
*/
int player_spawn(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	srand(time(NULL));

	if (get_nb_players_in_team(m, p) + 1 > NB_PLAYERS_MAX) {
		log_err("Too many players in this team");
		return -1;
	}
	p->pos = find_spawn_pos(m);
	if (p->pos.row == -1) {
		log_err("No spawn position available");
		return -1;
	}
	if (update_player_target(rcs, m, p) == -1)
		return -1;

	p->id = p->team_id + (m->nbp << 8); /* team_id 1 byte, play_id 3 bytes */
	spawn_update_map(m, p);

	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	printf("[ INFO  ] Player spawned (row %d, col %d)\n", p->pos.row + 1,
			p->pos.col + 1);
	return 0;
}