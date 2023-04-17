/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_move.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/02 15:49:33 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/17 15:25:51 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/player_move.h"

#include "../include/player_spawn.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TIME_BETWEEN_MOVE 1

static inline void player_move_left(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.col--;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void player_move_right(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.col++;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void player_move_up(struct mapinfo *m, struct player *p)
{
	set_id(m, p->pos.row, p->pos.col, 0);
	p->pos.row--;
	set_id(m, p->pos.row, p->pos.col, p->id);
}

static inline void player_move_down(struct mapinfo *m, struct player *p)
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

/**
 * Fill the buffer with team ids of ennemies surrounding the player. Ex (where
 * x is the player) :
 *     2 0 0
 *     0 x 4   ->>> Will fill the buffer with 2,4,3,2.
 *     3 2 0
*/
static inline void find_surrounding_ennemies(struct mapinfo *m,
					     const struct player *p,
					     char *squares_tids)
{
	int i = -1;
	char ennemy_tid;

	for (int row = p->pos.row - 1; row <= p->pos.row + 1; ++row) {
		if (!(row >= 0 && row < MAP_NB_ROWS))
			continue;
		for (int col = p->pos.col - 1; col <= p->pos.col + 1; ++col) {
			if (!(col >= 0 && col < MAP_NB_COLS))
				continue;
			if (row == p->pos.row && col == p->pos.col)
				continue;

			ennemy_tid = (char)get_id(m, row, col);
			if (ennemy_tid > 0 && (char)p->team_id != ennemy_tid)
				squares_tids[++i] = ennemy_tid;
		}
	}
}

/**
 * is_player_dead() - Unspawn the player if this one is surrounded by at least
 *                    2 ennemies from the same team.
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
int is_player_dead(struct mapinfo *m, const struct player *p)
{
	/* represent squares around player position + '\0'*/
	char squares_tids[9] = {};
	int i = -1;
	int game_state = E_STATE_PLAY;

	find_surrounding_ennemies(m, p, squares_tids);
	while (squares_tids[++i]) {
		if (strchr(&squares_tids[i + 1], squares_tids[i]) != NULL) {
			game_state = E_STATE_DEAD;
			break;
		}
	}
	if (game_state == E_STATE_DEAD) {
		player_unspawn(m, p);
		sleep(TIME_BETWEEN_MOVE);
		m->time_last_move.tv_sec += TIME_BETWEEN_MOVE;
		printf("[ INFO  ] You died surrounded by 2 ennemies from team "
				"%d (row %d, col %d)\n", (int)squares_tids[i],
				p->pos.row + 1, p->pos.col + 1);
	}
	return game_state;
}

/**
 * Do a random move on an empty square (can be left, right, up or down).
*/
static inline void random_move(struct mapinfo *m, struct player *p)
{
	int (*fptr_get_team_id[4])(const struct mapinfo *, int, int) = {
		&get_left_team_id,
		&get_right_team_id,
		&get_up_team_id,
		&get_down_team_id
	};
	void (*fptr_player_move[4])(struct mapinfo *, struct player *) = {
		&player_move_left,
		&player_move_right,
		&player_move_up,
		&player_move_down
	};
	int i;

	do {
		i = rand() % 4; /* rand was seeded in player_spawn() */
	} while (fptr_get_team_id[i](m, p->pos.row, p->pos.col) != 0);
	fptr_player_move[i](m, p);
}

static inline void update_player_pos(struct mapinfo *m, struct player *p,
		struct position ennemy_pos)
{
	int i = 0;
	int move_row = ennemy_pos.row - p->pos.row;
	int move_col = ennemy_pos.col - p->pos.col;
	void (*fptr_player_move[2])(struct mapinfo *, struct player *) = {};

	printf("[ INFO  ] Player moved (row %d, col %d) => ", p->pos.row + 1,
	       p->pos.col + 1);
	if (p->pos.row + move_row < 0 || p->pos.row + move_row >= MAP_NB_ROWS)
		move_row = 0;
	if (p->pos.col + move_col < 0 || p->pos.col + move_col >= MAP_NB_COLS)
		move_col = 0;

	if (move_row > 0 && !get_id(m, p->pos.row + 1, p->pos.col))
		fptr_player_move[i++] = &player_move_down;
	if (move_row < 0 && !get_id(m, p->pos.row - 1, p->pos.col))
		fptr_player_move[i++] = player_move_up;
	if (move_col > 0 && !get_id(m, p->pos.row, p->pos.col + 1))
		fptr_player_move[i++] = player_move_right;
	if (move_col < 0 && !get_id(m, p->pos.row, p->pos.col - 1))
		fptr_player_move[i++] = player_move_left;

	if (i == 1) {
		fptr_player_move[0](m, p);
	} else if (i == 2) {
		i = rand() % 2;
		fptr_player_move[i](m, p);
	} else {
		random_move(m, p);
	}
	printf("(row %d, col %d)\n", p->pos.row + 1, p->pos.col + 1);
}

int player_move(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	struct position ennemy_pos;
	struct timespec time_elapsed;
	struct timespec now;

	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	time_elapsed = sub_timespec(now, m->time_last_move);
	if (m->game_state == E_STATE_WON) {
		log_info("You won the game!");
	} else if (!is_surrounded(m, p)  &&
		   time_elapsed.tv_sec >= TIME_BETWEEN_MOVE) {

		ennemy_pos = find_player_pos(m, p->targ_id);
		if (ennemy_pos.row == -1) {
			log_verb("Finding new target");
			if (update_player_target(rcs, m, p) == -1)
				return -1;

			ennemy_pos = find_player_pos(m, p->targ_id);
			if (ennemy_pos.row == -1)
				log_verb("Could'nt find a target");
		}
		if (ennemy_pos.row != -1)
			update_player_pos(m, p, ennemy_pos);
		m->time_last_move = now;
		m->game_state = E_STATE_PRINT;
	}
	return m->game_state;
}
