/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:51:01 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 19:12:36 by llefranc         ###   ########.fr       */
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
volatile _Bool g_is_sem_locked = 0;

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
 * Try to join the game by checking if the game has already started or not.
*/
static int join_game(const struct shrcs *rcs, struct mapinfo *m,
		_Bool is_graphic_mode)
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

static inline int check_nb_players(const struct shrcs *rcs,
				   const struct mapinfo *m)
{
	int maxNbPlayer = 0;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	for (int team_nb = 0; team_nb < NB_TEAMS_MAX; ++team_nb) {
		if (m->nbp_team[team_nb] > maxNbPlayer)
			maxNbPlayer = m->nbp_team[team_nb];
	}
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	if (maxNbPlayer < 2)
		return -1;
	return 0;
}

/**
 * Wait SEC_START_TIME by printing the countdown on screen. Other players can
 * during this time join the game.
*/
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

	if (check_nb_players(rcs, m) == -1) {
		log_err("Only teams with one player");
		return -1;
	}
	log_info("Launching game!");
	return 0;
}

/**
 * Init time_last_move at the beggining of the game.
*/
static inline int init_time_last_move(const struct shrcs *rcs,
				      struct mapinfo *m)
{
	struct timespec now;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	m->time_last_move = now;
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	return 0;
}

/**
 * Print the team number of the team who is
*/
static inline int display_winner(const struct shrcs *rcs, struct mapinfo *m)
{
	int winner = 0;

	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	if ((winner = get_winner_team_id(m)) != 0)
		printf("[ INFO  ] Team %d won the game !\n", winner);
	else
		printf("[ INFO  ] Nobody joined the game\n");
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	return 0;
}

/**
 * Display the map, refreshing it each time a player moves or dies.
*/
int graphic_mode(const struct shrcs *rcs, struct mapinfo *m)
{
	int still_playing = 0;

	log_info("Graphic mode started");
	if ((join_game(rcs, m, 1) == -1) || (wait_for_players(rcs, m) == -1))
		goto err_exit;
	if (init_time_last_move(rcs, m) == -1)
		goto err_exit;

	do {
		if (is_sig_received)
			goto err_exit;
		if (sem_lock(rcs->sem_id) == -1)
			goto err_exit;
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
			goto err_exit;
	} while (still_playing > 1 && !is_sig_received);

	if (display_winner(rcs, m) == -1)
		goto err_exit;
	return 0;

err_exit:
	if (!g_is_sem_locked)
		sem_lock(rcs->sem_id);
	m->is_graphic_on = 0;
	sem_unlock(rcs->sem_id);
	return -1;
}

/**
 * Move the player towards ennemies until he dies or win.
*/
int player_mode(const struct shrcs *rcs, struct mapinfo *m, struct player *p)
{
	int game_state = E_STATE_PLAY;

	log_info("Player mode started");
	if (((join_game(rcs, m, 0)) == -1) || (spawn_player(rcs, m, p) == -1))
		return -1;
	if (wait_for_players(rcs, m) == -1)
		goto err_unspawn_player;

	while (game_state == E_STATE_PLAY || game_state == E_STATE_PRINT) {
		if (is_sig_received)
			goto err_unspawn_player;
		if (sem_lock(rcs->sem_id) == -1)
			goto err_unspawn_player;
		if (!m->is_graphic_on) {
			log_err("The graphic process isn't running");
			return -1;
		}
		if (m->game_state != E_STATE_PRINT) {
			if (is_player_dead(m, p))
				game_state = E_STATE_DEAD;
			else if ((game_state = move_player(rcs, m, p)) == -1)
				goto err_unspawn_player;
		}
		if (sem_unlock(rcs->sem_id) == -1)
			goto err_unspawn_player;
	}
	return 1;

err_unspawn_player:
	if (!g_is_sem_locked)
		sem_lock(rcs->sem_id);
	unspawn_player(m, p);
	sem_unlock(rcs->sem_id);
	return -1;
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

	if (is_sig_received)
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