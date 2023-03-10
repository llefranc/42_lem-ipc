/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn_player.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:27:54 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/10 13:16:35 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/spawn_player.h"

#include "../include/log.h"

/**
 * is_valid_spawn() - Checks if a player can spawn on a square from the grid.
 * @m: Contains all the map information.
 * @row: Row number of the map.
 * @col: Col number of the map.
 *
 * To be a valid spawn, the square must be surrounded by no other player (up,
 * down, left and right).
 *
 * Return: 1 if the spawn is valid, 0 otherwise.
*/
static _Bool is_valid_spawn(const struct mapinfo *m, int row, int col)
{
	_Bool up = 1;
	_Bool down = 1;
	_Bool left = 1;
	_Bool right = 1;

	if (col - 1 >= 0)
		left = !get_id(m, row, col - 1);
	if (col + 1 < MAP_NB_COLS)
		right = !get_id(m, row, col + 1);

	if (row - 1 >= 0)
		up = !get_id(m, row - 1, col);
	if (row + 1 < MAP_NB_ROWS)
		down = !get_id(m, row + 1, col);

	return !get_id(m, row, col) && up && down && left && right;
}

/**
 * find_spawn_pos() - Finds a valid spawn position.
 * @m: Contains all the map information.
 *
 * Tries to find a valid (i.e not surrounded by any other player in up, down,
 * left and right position) random spawn x times (x = MAP_NB_ROWS *
 * MAP_NB_COLS). If no random spawn is found after x times, then iterates
 * through and checks all squares.
 *
 * Return: A valid position if a spawn was found, a position of -1,-1 otherwise.
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
 * spawn_player() - Spawns a player on the grid.
 * @rcs: Contains all information for shared ressources.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Spawns a player on the grid if there is a square available (i.e. with no
 * other player at left, right, up and down position). For odd team, it will
 * start to look for an available square from the top of the map; for pair team
 * it will start to look from the bottom.
 * If there is a position available, then:
 *     - Look for a potential ennemy to target
 *     - Updates the map information
 *     - Updates the player information
 *
 * Return: 0 on success, -1 if an error occured.
*/
int spawn_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	srand(m->start_time.tv_sec);

	if (get_nb_players_in_team(m, p) + 1 > NB_PLAYERS_MAX) {
		log_err("Too many players in this team");
		goto err_unlock_sem;
	}
	p->pos = find_spawn_pos(m);
	if (p->pos.row == -1) {
		log_err("No spawn position available");
		goto err_unlock_sem;
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

err_unlock_sem:
	sem_unlock(rcs->sem_id);
	return -1;
}