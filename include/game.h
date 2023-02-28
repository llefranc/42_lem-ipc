/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:28:07 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/27 18:28:36 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_H
#define GAME_H

#define NB_PLAYERS_MAX 10
#define MAP_SIZE 100

struct mapinfo {
	int nbp;
	char nbp_team[NB_PLAYERS_MAX];
	int map[MAP_SIZE];
};

#endif /* GAME_H */