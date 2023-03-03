/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/03 15:44:05 by llefranc         ###   ########.fr       */
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

static inline int wait_for_players(const struct shrcs *rcs,
		const struct mapinfo *m)
{
	time_t now;
	time_t tmp;
	time_t time_since_start = SEC_START_TIME;
	time_t start_time;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	start_time = m->start_time;
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;

	if ((now = time(NULL)) == ((time_t) -1)) {
		log_syserr("(time)");
		return -1;
	}
	if (now - start_time > SEC_START_TIME) {
		log_info("Game has already started, impossible to join");
		return 0;
	}
	printf("[ INFO  ] Waiting for players to join, game will start in ");
	fflush(stdout);
	do {
		tmp = now - start_time;
		if (tmp != time_since_start) {
			time_since_start = tmp;
			printf("%ld... ", SEC_START_TIME - time_since_start);
			fflush(stdout);
		}
		if ((now = time(NULL)) == ((time_t) -1)) {
			log_syserr("(time)");
			return -1;
		}
	}
	while (time_since_start < SEC_START_TIME);
	printf("\n");
	return 1;
}


int graphic_mode(const struct shrcs *rcs, const struct mapinfo *m)
{
	int ret;
	int still_playing = 0;

	log_info("Graphic mode started");
	if ((ret = wait_for_players(rcs, m)) < 1)
		return ret;

	do {
		if (sem_lock(rcs->sem_id) == -1)
			return -1;
		print_map(m);
		still_playing = nb_teams_in_game(m);

		if (sem_unlock(rcs->sem_id) == -1)
			return -1;
	} while (still_playing > 1 && !is_sig_received);

	if (is_sig_received) {
		log_info("Signal received, quitting");
	} else {
		if (sem_lock(rcs->sem_id) == -1)
			return -1;
		print_map(m);
		printf("[ INFO  ] Team %d won the game !\n", get_winner(m));
		if (sem_unlock(rcs->sem_id) == -1)
			return -1;
	}
	return 0;
}

int player_mode(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	int ret;

	if (spawn_player(rcs, m, p) == -1)
		return -1;
	if ((ret = wait_for_players(rcs, m)) < 1)
		return ret;
	return 0;
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
		goto fatal_err_clean_all_rcs;

	p.team_id = (unsigned int)team_id;
	if (!team_id) {
		if (graphic_mode(&rcs, m) == -1)
			goto fatal_err_clean_all_rcs;
	} else {
		if (player_mode(&rcs, m, &p) == -1)
			goto fatal_err_clean_all_rcs;
	}

	if (clean_shared_rcs(&rcs, E_CLEAN_ALL) < 0)
		return 1;
	return 0;

fatal_err_clean_all_rcs:
	log_err("Exit because of an internal error");
	clean_shared_rcs(&rcs, E_CLEAN_ALL);
	return 1;
}