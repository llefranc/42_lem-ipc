/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/10 13:30:00 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/ipc.h>
#include <signal.h>
#include <stdlib.h>

#include "../include/shared_rcs.h"
#include "../include/game_utils.h"
#include "../include/spawn_player.h"
#include "../include/move_player.h"

static _Bool is_sig_received = 0;

static void sighandler(int signum)
{
	(void)signum;
	is_sig_received = 1;
}

static inline int setup_sighandlers(void)
{
	int sigs[1] = { SIGINT };

	for (size_t i = 0; i < sizeof(sigs) / sizeof(*sigs); ++i) {
		if (signal(SIGINT, sighandler) == SIG_ERR) {
			log_syserr("(signal)");
			return -1;
		}
	}
	return 0;
}

static inline int parse_team_id(int ac, char **av)
{
	long team_id = 0;
	char *p = NULL;

	if (ac != 2) {
		log_err("Enter only the team number as argument");
		return -1;
	}
	errno = 0;
	team_id = strtol(av[1], &p, 10);
	if (errno == ERANGE || (team_id < 0 || team_id > NB_TEAMS_MAX)
			|| *p != '\0') {
		fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m Team number must "
				"be a number between 0 and %d\n", NB_TEAMS_MAX);
		return -1;
	}
	return (int)team_id;
}

static int join_game(const struct shrcs *rcs, const struct mapinfo *m)
{
	struct timespec res;
	struct timespec now;
	struct timespec start_time;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	start_time = m->start_time;
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;

	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	res = sub_timespec(now, start_time);
	if (res.tv_sec > SEC_START_TIME) {
		log_info("Game has already started, impossible to join");
		return 0;
	}
	return 1;
}

static inline int wait_for_players(const struct shrcs *rcs,
		const struct mapinfo *m)
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
	while (time_elapsed.tv_sec < SEC_START_TIME && !is_sig_received);
	printf("\n");
	if (is_sig_received)
		return -1;
	log_info("Launching game!");
	return 0;
}

int graphic_mode(const struct shrcs *rcs, struct mapinfo *m)
{
	int ret;
	int winner = 0;
	int still_playing = 0;

	log_info("Graphic mode started");
	if ((ret = join_game(rcs, m)) < 1)
		return ret;
	if (wait_for_players(rcs, m) == -1)
		return -1;
	do {
		if (sem_lock(rcs->sem_id) == -1)
			return -1;

		still_playing = nb_teams_in_game(m);
		if (m->game_state == E_STATE_PRINT) {
			if (still_playing <= 1) {
				m->game_state = E_STATE_WON;
			} else {
				print_map(m);
				m->game_state = E_STATE_PLAY;
			}
		}
		if (sem_unlock(rcs->sem_id) == -1)
			return -1;
	} while (still_playing > 1 && !is_sig_received);

	if (!is_sig_received) {
		if (sem_lock(rcs->sem_id) == -1)
			return -1;

		if ((winner = get_winner(m)) != 0)
			printf("[ INFO  ] Team %d won the game !\n", winner);
		else
			printf("[ INFO  ] Nobody joined the game\n");

		if (sem_unlock(rcs->sem_id) == -1)
			return -1;
	}
	return 0;
}

int player_mode(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	int ret;
	int game_state = E_STATE_PLAY;

	log_info("Player mode started");
	if ((ret = join_game(rcs, m)) < 1)
		return ret;
	if (spawn_player(rcs, m, p) == -1)
		return -1;
	if (wait_for_players(rcs, m) == -1)
		goto err_unspawn_player;
		
	while (game_state == E_STATE_PLAY && !is_sig_received) {

		if (sem_lock(rcs->sem_id) == -1)
			goto err_unspawn_player;
		if (m->game_state != E_STATE_PRINT) {
			if (is_player_dead(m, p))
				game_state = E_STATE_DEAD;
			else if ((game_state = move_player(rcs, m, p)) == -1)
				goto err_unspawn_player_sem_locked;
		}
		if (sem_unlock(rcs->sem_id) == -1)
			goto err_unspawn_player_sem_locked;
	}
	return 1;

err_unspawn_player:
	sem_lock(rcs->sem_id);
err_unspawn_player_sem_locked:
	unspawn_player(m, p);
	sem_unlock(rcs->sem_id);
	log_err("Game error");
	return -1;
}

int main(int ac, char **av)
{
	key_t key;
	int team_id;
	struct shrcs rcs = {};
	struct mapinfo *m;
	struct player p = {};

	if (setup_sighandlers() == -1)
		return 1;
	if ((team_id = parse_team_id(ac, av)) == -1)
		return 1;

	if ((key = keygen(1)) == -1)
		return 1;
	if (get_shared_rcs(&rcs, key, sizeof(struct mapinfo)) == -1)
		return 1;
	if (init_shared_rcs(&rcs, &m) == -1)
		goto err_clean_all_rcs;

	p.team_id = (unsigned int)team_id;
	if (!team_id && graphic_mode(&rcs, m) == -1)
		goto err_clean_all_rcs;
	if (team_id && player_mode(&rcs, m, &p) == -1)
		goto err_clean_all_rcs;

	if (is_sig_received)
		log_info("Signal received, quitting");
	if (clean_shared_rcs(&rcs, E_CLEAN_ALL) < 0)
		return 1;
	return 0;

err_clean_all_rcs:
	clean_shared_rcs(&rcs, E_CLEAN_ALL);
	return 1;
}