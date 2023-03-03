/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 15:54:17 by llefranc         ###   ########.fr       */
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
	if (errno == ERANGE || (team_id < 1 || team_id > 9) || *p != '\0') {
		log_err("Team number must be a number between 1 and 9");
		return -1;
	}
	return (int)team_id;
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
	if (init_shared_rcs(&rcs) == -1)
		goto fatal_err_clean_all_rcs;

	m = (struct mapinfo *)rcs.shm_addr;
	p.team_id = (unsigned int)team_id;
	if (spawn_player(&rcs, m, &p) == -1) {
		print_map(m);
		goto fatal_err_clean_all_rcs;
	}
	print_map(m);

	if (clean_shared_rcs(&rcs, E_CLEAN_ALL) < 0)
		return 1;
	return 0;

fatal_err_clean_all_rcs:
	clean_shared_rcs(&rcs, E_CLEAN_ALL);
	return 1;
}