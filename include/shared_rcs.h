/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:39:35 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 15:04:19 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_RCS
#define SHARED_RCS

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "../include/utils.h"

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

struct msgbuf {
	long team_id;
	char targ_id[4];
};

static inline int sem_lock(int sem_id)
{
	int ret;
	struct sembuf b = { .sem_op = -1 };

	if ((ret = semop(sem_id, &b, 1)) == -1)
		log_syserr("(semop -1)");
	return ret;
}

static inline int sem_unlock(int sem_id)
{
	int ret;
	struct sembuf b = { .sem_op = 1 };

	if ((ret = semop(sem_id, &b, 1)) == -1)
		log_syserr("(semop 1)");
	return ret;
}

key_t keygen(int i);
int get_shared_rcs(struct shrcs *rcs, key_t key, size_t shmsize);
int init_shared_rcs(const struct shrcs *rcs);
int get_shm_nattch(int shm_id);
int clean_shared_rcs(const struct shrcs *rcs, enum clean_step step);

#endif /* SHARED_RCS */