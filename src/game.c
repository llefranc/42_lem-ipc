/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 16:06:56 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 12:02:47 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>

#include "../include/game.h"

#include "../include/utils.h"

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

struct pos find_player_pos(const struct mapinfo *m, unsigned int id)
{
	struct pos p = { .row = -1, .col = -1};

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