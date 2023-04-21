/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_lobby.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 19:35:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/21 16:07:45 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/game_lobby.h"

#include "../include/log.h"

#include <time.h>
#include <stdio.h>

extern _Bool g_is_sig_received;

/**
 * Try to join the game by checking if the game has already started or not.
*/
int join_game(const struct shrcs *rcs, struct mapinfo *m, _Bool is_graphic_mode)
{
	struct timespec res;
	struct timespec now;
	struct timespec start_time;

	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	start_time = m->start_time;
	res = sub_timespec(now, start_time);
	if (res.tv_sec > SEC_START_TIME) {
		log_info("Game has already started, impossible to join");
		return -1;
	} else if (is_graphic_mode && m->is_graphic_on) {
		log_info("The graphic process is already launched");
		return -1;
	} else if (is_graphic_mode) {
		m->is_graphic_on = 1;
	}
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	return 0;
}

/**
 * Return 0 if there is at least a team with 2 or more players, -1 otherwise.
*/
static inline int check_nb_players(const struct shrcs *rcs,
				   const struct mapinfo *m)
{
	int maxNbPlayer = 0;
	int nbTeam = 0;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	for (int team_nb = 0; team_nb < NB_TEAMS_MAX; ++team_nb) {
		if (m->nbp_team[team_nb] > maxNbPlayer)
			maxNbPlayer = m->nbp_team[team_nb];
		if (m->nbp_team[team_nb] > 0)
			++nbTeam;
	}
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	if (nbTeam < 2 || maxNbPlayer < 2)
		return -1;
	return 0;
}

/**
 * Wait SEC_START_TIME by printing the countdown on screen. Other players can
 * during this time join the game.
*/
int wait_for_players(const struct shrcs *rcs, const struct mapinfo *m)
{
	struct timespec now;
	struct timespec tmp;
	struct timespec time_elapsed = { .tv_sec = SEC_START_TIME };
	struct timespec start_time;

	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	start_time = m->start_time;
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;

	printf("[ INFO  ] Waiting for players to join, game will start in ");
	fflush(stdout);
	do {
		tmp = sub_timespec(now, start_time);
		if (tmp.tv_sec != time_elapsed.tv_sec) {
			time_elapsed = tmp;
			printf("%ld... ", SEC_START_TIME - time_elapsed.tv_sec);
			fflush(stdout);
		}
		if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
			log_syserr("(clock_gettime)");
			return -1;
		}
	}
	while (time_elapsed.tv_sec < SEC_START_TIME && !g_is_sig_received);
	printf("\n");
	if (g_is_sig_received)
		return -1;

	if (check_nb_players(rcs, m) == -1) {
		log_err("Not enough players");
		return -1;
	}
	log_info("Launching game!");
	return 0;
}
