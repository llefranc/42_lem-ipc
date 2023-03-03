/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 16:06:56 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/03 12:11:07 by llefranc         ###   ########.fr       */
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

#include "../include/game_utils.h"

#include "../include/log.h"
#include "../include/shared_rcs.h"

static char *colors[8] = {
	"\x1B[0m",
	"\x1B[31;1m",
	"\x1B[32;1m",
	"\x1B[33;1m",
	"\x1B[34;1m",
	"\x1B[35;1m",
	"\x1B[36;1m",
	"\x1B[37;1m"
};

static inline void print_team_id(unsigned int team_id)
{
	if (team_id)
		printf(" %s%u\x1B[0m |" , colors[team_id], team_id);
	else
		printf("   |");
}

static inline void print_team_id_line(const struct mapinfo *m, int row)
{
	unsigned int team_id;

	printf("\t|");
	for (int col = 0; col < MAP_NB_COLUMNS; ++col) {
		team_id = get_team_id(m, row, col);
		print_team_id(team_id);
	}
	printf("\n");
}

static inline void print_grid_line(void)
{
	printf("\t-");
	for (int col = 0; col < MAP_NB_COLUMNS; ++col)
			printf("----");
	printf("\n");
}

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

	if (is_map_printed) {
		printf("\x1B[%dA", MAP_NB_ROWS * 2 + 7);
		fflush(stdout); /* remove previous map from terminal */
	}

	printf("\n");
	for (int row = 0; row < MAP_NB_ROWS; ++row) {
		print_grid_line();
		print_team_id_line(m, row);
	}
	print_grid_line();
	printf("\n");

	printf("\t TEAM       |");
	for (int team_nb = 1; team_nb <= NB_TEAMS_MAX; ++team_nb)
		print_team_id(team_nb);
	printf("\n\t-------------");
	for (int team_nb = 0; team_nb < NB_TEAMS_MAX; ++team_nb)
		printf("----");
	printf("\n\t NB PLAYERS |");
	for (int team_nb = 0; team_nb < NB_TEAMS_MAX; ++team_nb)
		printf(" %u |", (unsigned int)m->nbp_team[team_nb]);
	printf("\n\n");

	is_map_printed = 1;
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
		// log_verb("Received target id from message queue");
		return *(unsigned int *)buf.targ_id;

	} else if (errno != ENOMSG) {
		log_syserr("(msgrcv)");
		return -1;
	}
	// log_verb("No target id to receive from message queue");
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
	// log_verb("Send new target id in message queue");
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

	if (!get_id(m, row, col) ||
			is_in_team(get_id(m, row, col), p->team_id))
		return UINT_MAX;
	dist = abs(p->pos.row - row) + abs(p->pos.col - col);
	if (!dist)
		return UINT_MAX;
	return dist;
}

/**
 * update_player_target() - Update the player target with the id of the closest
 *                          ennemy.
 * @rcs: Contains all information for shared ressources.
 * @m: Contains all the map information.
 * @p: Contains the actual player information.
 *
 * Checks the message queue to see if a team member already sent the id of an
 * ennemy player to target. If not, looks on the board to find the closest
 * ennemy. If an ennemy is found, the target_id of player is update and a
 * message with this target id and the player team id is sent on the message
 * queue.
 *
 * Return: The identifiant of an ennemy player as a positive integer, 0 if there
 *         is no ennemy to target, -1 if an error occured.
*/
int update_player_target(const struct shrcs *rcs, const struct mapinfo *m,
		struct player *p)
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
				targ_id = get_id(m, row, col);
			}
		}
	}
	p->targ_id = (unsigned int)targ_id;
	// if (!p->targ_id)
	// 	log_verb("Could'nt find a new target on map");
	// else if (send_targ_id(rcs->msgq_id, p->team_id, p->targ_id) == -1)
	if (send_targ_id(rcs->msgq_id, p->team_id, p->targ_id) == -1)
		return -1;

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
			if (get_id(m, row, col) == id) {
				set_pos(&p, row, col);
				return p;
			}
		}
	}
	return p;
}