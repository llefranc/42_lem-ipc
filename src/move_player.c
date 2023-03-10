/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   move_player.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:49:33 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/10 15:04:41 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/move_player.h"

#include "../include/spawn_player.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TIME_BETWEEN_MOVE 1

static inline void move_player_left(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.col--;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void move_player_right(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.col++;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void move_player_up(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.row--;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void move_player_down(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.row++;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

_Bool is_surrounded(const struct mapinfo *m, const struct player *p)
{
	int (*fptr[4])(const struct mapinfo *, int, int) = {
		&get_left_team_id,
		&get_right_team_id,
		&get_up_team_id,
		&get_down_team_id,
	};

	for (size_t i = 0; i < sizeof(fptr) / sizeof(*fptr); ++i) {
		if (!fptr[i](m, p->pos.row, p->pos.col))
			return 0;
	}
	return 1;
}

_Bool is_target_reached(const struct mapinfo *m, const struct player *p)
{
	unsigned int (*fptr[4])(const struct mapinfo *, int, int) = {
		&get_left_id,
		&get_right_id,
		&get_up_id,
		&get_down_id,
	};

	if (!p->targ_id)
		return 0;
	for (size_t i = 0; i < sizeof(fptr) / sizeof(*fptr); ++i) {
		if (fptr[i](m, p->pos.row, p->pos.col) == p->targ_id)
			return 1;
	}
	return 0;
}

/**
 * is_dead() - Unspawns the player if this one is surrounded by at least 2
 *             ennemies from the same team.
 * @rcs: Contains all information for shared ressources.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Unspawns a player if this is surrounded by at least 2 ennemies from the same
 * team on the up/down/left/right squares.
 *
 * Return: 1 if the player is surrounded by at 2 ennemies from the same team.
 *         0 otherwise. Note that if the player is surrounded by 2 ennemies but
 *         not from the same team, the function will return 0.
*/
int is_dead(const struct shrcs *rcs, struct mapinfo *m, const struct player *p)
{
	/* represent up, down, left, and right squares + '\0'*/
	char squares_tids[5] = {};
	int (*fptr[4])(const struct mapinfo *, int, int) = {
		&get_left_team_id,
		&get_right_team_id,
		&get_up_team_id,
		&get_down_team_id,
	};
	int i = -1;
	int game_state = E_STATE_PLAY;
	int ennemy_tid;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;

	for (size_t j = 0; j < sizeof(fptr) / sizeof(*fptr); ++j) {

		ennemy_tid = fptr[j](m, p->pos.row, p->pos.col);
		if (ennemy_tid > 0 && (int)p->team_id != ennemy_tid)
			squares_tids[++i] = (char)ennemy_tid;
	}
	i = -1;
	while (squares_tids[++i]) {
		if (strchr(&squares_tids[i + 1], squares_tids[i]) != NULL) {
			game_state = E_STATE_DEAD;
			unspawn_player(m, p);
			sleep(TIME_BETWEEN_MOVE);
			m->time_last_move.tv_sec += TIME_BETWEEN_MOVE;
			break;
		}
	}
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;

	if (game_state == E_STATE_DEAD) {
		printf("[ INFO  ] You died surrounded by 2 ennemies from team "
				"%d (row %d, col %d)\n", (int)squares_tids[i],
				p->pos.row + 1, p->pos.col + 1);
	}
	return game_state;
}

static inline void random_move(struct mapinfo *m, struct player *p)
{
	int (*fptr_get_team_id[4])(const struct mapinfo *, int, int) = {
		&get_left_team_id,
		&get_right_team_id,
		&get_up_team_id,
		&get_down_team_id
	};
	void (*fptr_move_player[4])(struct mapinfo *, struct player *) = {
		&move_player_left,
		&move_player_right,
		&move_player_up,
		&move_player_down
	};
	int i;

	do {
		i = rand() % 4; /* rand was seeded in spawn_player() */
	} while (fptr_get_team_id[i](m, p->pos.row, p->pos.col) != 0);
	fptr_move_player[i](m, p);
}

static inline void update_player_pos(struct mapinfo *m, struct player *p,
		struct position ennemy_pos)
{
	int move_row = ennemy_pos.row - p->pos.row;
	int move_col = ennemy_pos.col - p->pos.col;

	printf("Player teamid=%u, id=%u || targteamid=%u, targid=%u\n", p->team_id, p->id, (unsigned int)((char)p->targ_id), p->targ_id);

	// printf("enn pos: row=%d, col=%d\n", ennemy_pos.row, ennemy_pos.col);
	// printf("old pos: row=%d, col=%d\n", p->pos.row, p->pos.col);
	// printf("move_row = %d, move_col = %d\n", move_row, move_col);

	/* if dist is 1 == ennemy is next square, no need to move */ // >> to remove
	if (p->pos.row + move_row < 0 || p->pos.row + move_row >= MAP_NB_ROWS)
		move_row = 0;
	if (p->pos.col + move_col < 0 || p->pos.col + move_col >= MAP_NB_COLS)
		move_col = 0;

	// printf("move_row = %d, move_col = %d\n", move_row, move_col);

	printf("[ INFO  ] Player moved (row %d, col %d) => ", p->pos.row + 1,
			p->pos.col + 1);

	if (move_row > 0 && !get_id(m, p->pos.row + 1, p->pos.col))
		move_player_down(m, p);
	else if (move_row < 0 && !get_id(m, p->pos.row - 1, p->pos.col))
		move_player_up(m, p);
	else if (move_col > 0 && !get_id(m, p->pos.row, p->pos.col + 1))
		move_player_right(m, p);
	else if (move_col < 0 && !get_id(m, p->pos.row, p->pos.col - 1))
		move_player_left(m, p);
	else
		random_move(m, p);

	printf(" (row %d, col %d)\n", p->pos.row + 1, p->pos.col + 1);
}

int move_player(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	int ret = E_STATE_PLAY;
	struct position ennemy_pos;
	struct timespec time_elapsed;
	struct timespec now;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;

	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	time_elapsed = sub_timespec(now, m->time_last_move);
	if (m->game_state == E_STATE_PLAY && !is_surrounded(m, p) &&
	    time_elapsed.tv_sec >= TIME_BETWEEN_MOVE) {

		// printf("istargetreached=%d\n", is_target_reached(m, p));

		ennemy_pos = find_player_pos(m, p->targ_id);
		if (ennemy_pos.row == -1) {
			log_verb("Finding new target");
			if (update_player_target(rcs, m, p) == -1)
				goto err_unlock_sem;

			ennemy_pos = find_player_pos(m, p->targ_id);
			if (ennemy_pos.row == -1)
				log_verb("Could'nt find a target");
		}
		if (ennemy_pos.row != -1)
			update_player_pos(m, p, ennemy_pos);
		m->time_last_move = now;
		m->game_state = E_STATE_PRINT;

	} else if (m->game_state == E_STATE_WON) {
		ret = E_STATE_WON;
	}

	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	if (ret == E_STATE_WON)
		log_info("You won the game!");
	return ret;

err_unlock_sem:
	sem_unlock(rcs->sem_id);
	return -1;
}
