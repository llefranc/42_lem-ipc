/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_mode.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 20:00:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/05/31 16:27:49 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/game_mode.h"

#include "../include/log.h"
#include "../include/player_spawn.h"
#include "../include/player_move.h"
#include "../include/game_lobby.h"

#include <time.h>
#include <stdint.h>

extern _Bool g_is_sig_received;
extern _Bool g_is_sem_locked;

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
	if ((join_game(rcs, m, 1) == -1))
		goto err_exit_too_many_graph_procs;
	if ((wait_for_players(rcs, m) == -1) || 
	    init_time_last_move(rcs, m) == -1)
		goto err_exit;

	do {
		if (g_is_sig_received)
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
	} while (still_playing > 1);

	if (display_winner(rcs, m) == -1)
		goto err_exit;
	return 0;

err_exit_too_many_graph_procs:
	if (g_is_sem_locked)
		sem_unlock(rcs->sem_id);
	return -1;

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
	if (((join_game(rcs, m, 0)) == -1) || (player_spawn(rcs, m, p) == -1))
		return -1;
	if (wait_for_players(rcs, m) == -1)
		goto err_player_unspawn;

	while (game_state == E_STATE_PLAY || game_state == E_STATE_PRINT) {
		if (g_is_sig_received)
			goto err_player_unspawn;
		if (sem_lock(rcs->sem_id) == -1)
			goto err_player_unspawn;
		if (!m->is_graphic_on) {
			log_err("The graphic process isn't running");
			return -1;
		}
		if (m->game_state != E_STATE_PRINT) {
			if (is_player_dead(m, p))
				game_state = E_STATE_DEAD;
			else if ((game_state = player_move(rcs, m, p)) == -1)
				goto err_player_unspawn;
		}
		if (sem_unlock(rcs->sem_id) == -1)
			goto err_player_unspawn;
	}
	return 1;

err_player_unspawn:
	if (!g_is_sem_locked)
		sem_lock(rcs->sem_id);
	player_unspawn(m, p);
	sem_unlock(rcs->sem_id);
	return -1;
}
