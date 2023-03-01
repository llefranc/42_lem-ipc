/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 16:06:56 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 15:10:15 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <limits.h>

#include "../include/game.h"

#include "../include/utils.h"
#include "../include/shared_rcs.h"

static inline _Bool is_tmate(int team_id, int play_id);

int print_map(const struct mapinfo *m)
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
			printf("Team %d: %d players", team_nb + 1,
					(int)m->nbp_team[team_nb]);
		}
		printf("\n");
	}
	printf("\n");

	is_map_printed = 1;
	return 0;
}

static int send_targ_id(int msgq_id, unsigned int team_id, unsigned int targ_id)
{
	int ret;
	struct msgbuf buf = { .team_id = team_id };
	*(unsigned int *)buf.targ_id = targ_id;

	if ((ret = msgsnd(msgq_id, &buf, sizeof(buf.targ_id),
			IPC_NOWAIT)) == -1) {
		log_syserr("(msgsnd)");
	}
	return ret;
}

static int recv_targ_id(int msgq_id, unsigned int team_id)
{
	struct msgbuf buf = { .team_id = team_id };

	if (msgrcv(msgq_id, &buf, sizeof(buf.targ_id), buf.team_id,
			IPC_NOWAIT) != -1) {
		return *(unsigned int *)buf.targ_id;
	} else if (errno != ENOMSG) {
		log_syserr("(msgrcv)");
		return -1;
	}
	return 0;
}

static inline _Bool is_tmate(int team_id, int play_id)
{
	return team_id == (char)play_id;
}

static unsigned int calc_dist(const struct mapinfo *m,
		const struct player *p, int row, int col)
{
	unsigned int dist;

	if (!m->map[row][col] || is_tmate(p->team_id, m->map[row][col]))
		return UINT_MAX;
	dist = abs(p->pos.row - row) + abs(p->pos.col - col);
	if (!dist)
		return UINT_MAX;
	return dist;
}

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
			dist = calc_dist(m, p, row, col);
			if (closest > dist) {
				closest = dist;
				targ_id = m->map[row][col];
			}
		}
	}
	return targ_id;
}

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