/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_utils.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:28:07 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 15:45:15 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#define NB_PLAYERS_MAX 9
#define NB_TEAMS_MAX 7 /* because 7 ascii colors */
#define MAP_NB_ROWS 5
#define MAP_NB_COLS 5
#define SEC_START_TIME 30

#include <time.h>

#include "../include/shared_rcs.h"

struct shrcs;

enum game_state {
	E_STATE_PLAY,
	E_STATE_PRINT,
	E_STATE_WON,
	E_STATE_DEAD,
};

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
 * @game_state: The current state of the game.
 * @start_time: The time at which the first process joined the game (either in
 *              player mode or graphic mode).
 * @time_last_move: The time at which the last player move occured.
 * @nbp: The number of player which joined the game since it started.
 * @nbp_team: The number of players per team actually playing.
 * @map: The game map containing all the players.
 *
 * All fields of mapinfo are stored in the shared memory segment, which allows
 * each process to be able to see the actual game state and interact with it.
*/
struct mapinfo {
	int game_state;
	struct timespec start_time;
	struct timespec time_last_move;
	unsigned int nbp;
	unsigned char nbp_team[NB_TEAMS_MAX];
	unsigned int map[MAP_NB_ROWS][MAP_NB_COLS];
};

/**
 * struct player - Contains the player information.
 * @id: The id of the player resulting in the combination of its personnal id,
 *      which is the number of player who previously joined when he joined the
 *      game, and its team id (3 bytes for player id, 1 byte for team id).
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 * @targ_id: The id of an ennemy player that the player is actually targeting.
 * @pos: The player position on the grid.
*/
struct player {
	unsigned int id;
	unsigned int team_id;
	unsigned int targ_id;
	struct position pos;
};

/**
 * get_nb_players_in_team() - Gets the number of players in the player team.
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 * @play_id: The id of a player resulting in the combination of its personnal id
 *           and its team id (3 bytes for player id, 1 byte for team id).
 *
 * Return: The number of players in the player team (actual player include).
*/
static inline int get_nb_players_in_team(const struct mapinfo *m, const struct player *p)
{
	return m->nbp_team[p->team_id - 1];
}

static inline void set_pos(struct position *p, int row, int col)
{
	p->row = row;
	p->col = col;
}

static inline unsigned int get_id(const struct mapinfo *m, int row, int col)
{
	return m->map[row][col];
}

static inline void set_id(struct mapinfo *m, int row, int col, unsigned int id)
{
	m->map[row][col] = id;
}

static inline unsigned int get_team_id(const struct mapinfo *m, int row, int col)
{
	return (unsigned char)m->map[row][col];
}

/**
 * is_in_team() - Indicates if a player id belongs to a team or not.
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 * @play_id: The id of a player resulting in the combination of its personnal id
 *           and its team id (3 bytes for player id, 1 byte for team id).
 *
 * Return: 1 if the player belongs to the team, 0 otherwise.
*/
static inline _Bool is_in_team(unsigned int play_id, unsigned int team_id)
{
	return (unsigned char)play_id == (unsigned char)team_id;
}

struct timespec sub_timespec(struct timespec t1, struct timespec t2);
void print_map(const struct mapinfo *m);
int send_targ_id(int msgq_id, unsigned int team_id, unsigned int targ_id);
int update_player_target(const struct shrcs *rcs, const struct mapinfo *m,
		struct player *p);
struct position find_player_pos(const struct mapinfo *m, unsigned int id);
int nb_teams_in_game(const struct mapinfo *m);
int nb_players_in_game(const struct mapinfo *m);
int get_winner(const struct mapinfo *m);

#endif /* GAME_UTILS_H */