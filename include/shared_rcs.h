/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:39:35 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/28 14:28:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_RCS
#define SHARED_RCS

#include <sys/types.h>
#include <sys/ipc.h>

enum clean_step {
	E_CLEAN_SHM,
	E_CLEAN_SHM_SEM,
	E_CLEAN_ALL,
};

struct shrcs {
	int shm_id;
	int sem_id;
	int msgq_id;
	size_t shm_size;
	char *shm_addr;
};

key_t keygen(int i);
int get_shared_rcs(struct shrcs *rcs, key_t key, size_t shmsize);
int get_shm_nattch(int shm_id);
int clean_shared_rcs(struct shrcs *p, enum clean_step step);

#endif /* SHARED_RCS */