/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:28:07 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/28 18:32:18 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_H
#define GAME_H

#define NB_PLAYERS_MAX 9
#define NB_TEAMS_MAX 9
#define MAP_NB_ROWS 10
#define MAP_NB_COLUMNS 10

struct mapinfo {
	int nbp;
	char nbp_team[NB_TEAMS_MAX];
	int map[MAP_NB_ROWS][MAP_NB_COLUMNS];
};

int print_map(const struct mapinfo *m);

#endif /* GAME_H */