/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 16:06:56 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 15:43:42 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/game.h"

#include "../include/utils.h"
#include "../include/shared_rcs.h"

/**
 * print_map() - Refreshes the terminal with a representation of the game map.
 * @m: Contains all the map information.
 *
 * Prints the map and teams infos on the terminal, or refreshes it with new
 * infos in case it was already printed on the terminal.
*/
void print_map(const struct mapinfo *m)
{
	static _Bool is_map_printed = 0;
	char buf[(MAP_NB_COLUMNS * 2) + 2] = {}; /* +2 for 1st '\t' and '\0' */
	int team_nb = -1;

	if (is_map_printed) {
		printf("\x1B[%dA", (MAP_NB_ROWS) + 2);
		fflush(stdout); /* remove previous map from terminal */
	}

	printf("\n");
	for (int row = 0; row < MAP_NB_ROWS; ++row) {
		int i = -1;
		memset(buf, 0, sizeof(buf));
		buf[++i] = '\t';
		for (int col = 0; col < MAP_NB_COLUMNS; ++col) {

			/* Reading first byte only --> team id */
			buf[++i] = (char)m->map[row][col] + '0';
			buf[++i] = '|';
		}
		buf[i] = '\t';
		printf("%s", buf); /* print line map */

		if (++team_nb < NB_TEAMS_MAX) {
			printf("Team %d: %d player(s)", team_nb + 1,
					(int)m->nbp_team[team_nb]);
		}
		printf("\n");
	}
	printf("\n");

	is_map_printed = 1;
}

static inline void set_pos(struct position *p, int row, int col)
{
	p->row = row;
	p->col = col;
}

/**
 * recv_targ_id() - Receives a target id from a message queue.
 * @msqg_id: System V id of the message queue.
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 *
 * Receives a target id previously sent by another team member on a message
 * queue in order to target all together the same ennemy.
 *
 * Return: target id if a team member previously sent one, 0 if no target id
 *         for this specific team was on the message queue, -1 in case of error.
*/
static int recv_targ_id(int msgq_id, unsigned int team_id)
{
	struct msgbuf buf = { .team_id = team_id };

	if (msgrcv(msgq_id, &buf, sizeof(buf.targ_id), buf.team_id,
			IPC_NOWAIT) != -1) {
		log_verb("Received target id from message queue");
		return *(unsigned int *)buf.targ_id;

	} else if (errno != ENOMSG) {
		log_syserr("(msgrcv)");
		return -1;
	}
	log_verb("No target id to receive from message queue");
	return 0;
}

/**
 * send_targ_id() - Sends a target id on a message queue.
 * @msqg_id: System V id of the message queue.
 * @team_id: The team id of the player (lemipc arg, between 1 and 9).
 * @targ_id: The id of a player to target resulting in the combination of its
 *           personnal id and its team id (3 bytes for player id, 1 byte for
 *           team id).
 *
 * Sends a target id associated to a team id on a message queue, that other
 * players from same team can retrieve to target the same ennemy together.
 *
 * Return: 0 on success, -1 on failure.
*/
int send_targ_id(int msgq_id, unsigned int team_id, unsigned int targ_id)
{
	int ret;
	struct msgbuf buf = { .team_id = team_id };
	*(unsigned int *)buf.targ_id = targ_id;

	if ((ret = msgsnd(msgq_id, &buf, sizeof(buf.targ_id),
			IPC_NOWAIT)) == -1) {
		log_syserr("(msgsnd)");
	}
	log_verb("Send new target id in message queue");
	return ret;
}

/**
 * calc_dist_to_ennemy() - Calculates the distance between the player and
 *                         an ennemy.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 * @row: Row number of the map.
 * @col: Column number of the map.
 *
 * Calculates the distance between the player and a map's position but only if
 * it's an ennemy player which is standing on this square.
 *
 * Return: the distance in authorized moves (no diagonal moves) if it's an
 *         ennemy standing on the square from row/col. UINT max if it's a
 *         team member or if there is nobody.
*/
static unsigned int calc_dist_to_ennemy(const struct mapinfo *m,
		const struct player *p, int row, int col)
{
	unsigned int dist;

	if (!m->map[row][col] || is_in_team(m->map[row][col], p->team_id))
		return UINT_MAX;
	dist = abs(p->pos.row - row) + abs(p->pos.col - col);
	if (!dist)
		return UINT_MAX;
	return dist;
}

/**
 * find_new_target() - Finds the id of next ennemy to target.
 * @rcs: Contains all information for shared ressources.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Checks the message queue to see if a team member already sent the id of an
 * ennemy player to target. If not, looks on the board to find the closest
 * ennemy.
 *
 * Return: The identifiant of an ennemy player as a positive integer, 0 if there
 *         is no ennemy to target, -1 if an error occured.
*/
int find_new_target(const struct shrcs *rcs, const struct mapinfo *m,
		const struct player *p)
{
	int targ_id;
	unsigned int dist;
	unsigned int closest = UINT_MAX;

	if ((targ_id = recv_targ_id(rcs->msgq_id, p->team_id)) != 0)
		return targ_id;

	for (int row = 0; row < MAP_NB_ROWS; ++row) {
		for (int col = 0; col < MAP_NB_COLUMNS; ++col) {
			dist = calc_dist_to_ennemy(m, p, row, col);
			if (closest > dist) {
				closest = dist;
				targ_id = m->map[row][col];
			}
		}
	}
	return targ_id;
}

/**
 * find_player_pos() - Finds a player position based on its id.
 * @m: Contains all the map information.
 * @id: The id of a player resulting in the combination of its personnal id
 *      and its team id (3 bytes for player id, 1 byte for team id).
 *
 * Return: The position of the player if it exists on the map, or a position of
 *         {-1,-1} if not.
*/
struct position find_player_pos(const struct mapinfo *m, unsigned int id)
{
	struct position p = { .row = -1, .col = -1};

	if (!id)
		return p;
	for (int row = 0; row < MAP_NB_ROWS; ++row) {
		for (int col = 0; col < MAP_NB_COLUMNS; ++col) {
			if (m->map[row][col] == id) {
				p.row = row;
				p.col = col;
				return p;
			}
		}
	}
	return p;
}



