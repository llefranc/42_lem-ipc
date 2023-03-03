/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spawn_player.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:27:54 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 17:54:12 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <time.h>
#include <stdio.h>

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
		left = !m->map[row][col - 1];
	if (col + 1 < MAP_NB_COLUMNS)
		right = !m->map[row][col + 1];

	if (row - 1 >= 0)
		up = !m->map[row - 1][col];
	if (row + 1 < MAP_NB_ROWS)
		down = !m->map[row + 1][col];

	return !m->map[row][col] && up && down && left && right;
}

/**
 * find_spawn_pos() - Finds a valid spawn position based on team id.
 * @m: Contains all the map information.
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 *
 * Iterates through the map looking for a valid spawn position for a new player
 * (i.e not surrounded by any other player in up, down, left and right
 * position). The iteration will start from [row_min][col_min] for odd team id,
 * and in reverse order (from [row_max][col_max]) for pair team id.
 *
 * Return: A valid position if a spawn was found, a position of -1,-1 otherwise.
*/
static struct position find_spawn_pos(struct mapinfo *m, unsigned int team_id)
{
	int next = 1;
	int rstart = 0;
	int cstart = 0;
	int rend = MAP_NB_ROWS;
	int cend = MAP_NB_COLUMNS;
	struct position pos = { .row = -1, .col = -1 };

	if (team_id % 2 == 0) {
		next = -1;
		rstart = MAP_NB_ROWS - 1;
		cstart = MAP_NB_COLUMNS - 1;
		rend = -1;
		cend = -1;
	}
	for (int row = rstart; row != cend; row += next) {
		for (int col = cstart; col != rend; col += next) {
			if (is_valid_spawn(m, row, col)) {
				pos.row = row;
				pos.col = col;
				return pos;
			}
		}
	}
	return pos;
}

static inline int spawn_update_player(const struct mapinfo *m, struct player *p)
{
	if ((p->last_move = time(NULL)) == ((time_t) -1)) {
		log_syserr("(time)");
		return -1;
	}
	p->id = p->team_id + (m->nbp << 8); /* team_id 1 byte, play_id 3 bytes */
	return 0;
}

static inline void spawn_update_map(struct mapinfo *m, const struct player *p)
{
	m->nbp++;
	m->nbp_team[p->team_id - 1]++;
	m->map[p->pos.row][p->pos.col] = p->id;
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

	if (get_mates_nb(m, p) + 1 > NB_PLAYERS_MAX) {
		log_err("Too many players in this team");
		goto err_unlock_sem;
	}
	p->pos = find_spawn_pos(m, p->team_id);
	if (p->pos.row == -1) {
		log_err("No spawn position available");
		goto err_unlock_sem;
	}
	if (update_player_target(rcs, m, p) == -1)
		return -1;

	if (spawn_update_player(m, p) == -1)
		goto err_unlock_sem;
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