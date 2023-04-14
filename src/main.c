/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 20:10:36 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/log.h"
#include "../include/shared_rcs.h"
#include "../include/game_utils.h"
#include "../include/game_mode.h"

#include <sys/ipc.h>
#include <signal.h>
#include <stdlib.h>

_Bool g_is_sig_received = 0;
_Bool g_is_sem_locked = 0;

static void sighandler(int signum)
{
	(void)signum;
	g_is_sig_received = 1;
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

/**
 * Parse the first argument of lemipc which is team_id.
*/
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

/**
 * Init the shared ressources, and then launch graphic mode (which display the
 * map) if team_id == 0, otherwise launch player mode.
*/
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

	if (g_is_sig_received)
		log_info("Signal received, quitting");
	if (clean_shared_rcs(&rcs, E_CLEAN_ALL) < 0)
		return 1;
	return 0;

err_clean_all_rcs:
	if (g_is_sem_locked)
		sem_unlock(rcs.sem_id);
	clean_shared_rcs(&rcs, E_CLEAN_ALL);
	return 1;
}