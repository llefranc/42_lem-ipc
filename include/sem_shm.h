/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_shm.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 15:00:44 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/07 11:25:57 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEM_SHM_H
#define SEM_SHM_H

int init_shm(const int key, const int size, int *shm_id, char **shm_addr);
int clean_shm(int shm_id, char *shm_addr);
int get_shm_nattch(int shm_id);

#endif