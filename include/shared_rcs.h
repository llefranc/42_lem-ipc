/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:39:35 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/01 17:16:58 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_RCS
#define SHARED_RCS

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "../include/utils.h"

/**
 * enum clean_step - Describes which System V shared ressources needs to be
 *                   destroyed.
 * @E_CLEAN_SHM: To clean only the shared memory segment.
 * @E_CLEAN_SHM_SEM: To clean only the shared memory segment and the semaphore
 *                   set.
 * @E_CLEAN_ALL: To clean the shared memory segment, the semaphore set and the
 *               message queue.
*/
enum clean_step {
	E_CLEAN_SHM,
	E_CLEAN_SHM_SEM,
	E_CLEAN_ALL,
};

/**
 * struct shrcs - Contains all information of System V shared ressources.
 * @shm_id: The System V key to reference the shared memory segment.
 * @sem_id: The System V key to reference the semaphore set.
 * @msgq_id: The System V key to reference the message queue.
 * @shm_size: The size of the shared memory segment.
 * @shm_addr: The address to access the shared memory segment after it was
 *            attached to this process.
*/
struct shrcs {
	int shm_id;
	int sem_id;
	int msgq_id;
	size_t shm_size;
	char *shm_addr;
};

/**
 * struct msgbuf - Message sent on the message queue.
 * @team_id: Identify the message as addressed to a specific team.
 * @targ_id: An array of 4 char, which is casted to an int to put/retrieve
 *           the id of an ennemy player to target.
*/
struct msgbuf {
	long team_id;
	char targ_id[4];
};

/**
 * sem_lock() - Locks a semaphore.
 * @sem_id: The System V key to reference the semaphore set.
 *
 * Substracts 1 to the only semaphore of the semaphore set. If the semaphore is
 * 0 or less, this operation will block until other processes unlocks the
 * semaphore.
 *
 * Return: 0 on success, -1 on failure.
*/
static inline int sem_lock(int sem_id)
{
	int ret;
	struct sembuf b = { .sem_op = -1 };

	if ((ret = semop(sem_id, &b, 1)) == -1)
		log_syserr("(semop -1)");
	return ret;
}

/**
 * sem_lock() - Unlocks a semaphore.
 * @sem_id: The System V key to reference the semaphore set.
 *
 * Adds 1 to the only semaphore of the semaphore set. This operation will never
 * block.
 *
 * Return: 0 on success, -1 on failure.
*/
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