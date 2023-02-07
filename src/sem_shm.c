/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_shm.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 14:58:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/07 12:50:30 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "../include/utils.h"
#include "../include/sem_shm.h"

/**
 * init_shmparam() - Inits shmparam with all necessary information.
 * @shm: Will contain all informations for a specific shm.
 * @shm_key: Key used to identify the shm for shmget() call.
 * @shm_size: Size expected in bytes for the shm.
 * @sem_key: Key used to idendify the sem (obtained wtih semget() call)
 * 		associated to the shm.
 *
 * Initializes the struct shmparam with all necessary info to identify and use
 * a shm/sem couple. Allocates to smh->buf a tmp buffer used for read/write
 * operations with shm.
 *
 * Return: 0 on success, -1 on malloc failure.
*/
int init_shmparam(struct shmparam *shm, int shm_key, int shm_size, int sem_key)
{
	memset((void *)shm, 0, sizeof(*shm));
	shm->key = shm_key;
	shm->size = shm_size;
	shm->sem_key = sem_key;
	if ((shm->buf = malloc(sizeof(*(shm->buf)) * shm->size)) == NULL) {
		log_syserr("malloc");
		return -1;
	}
	return 0;
}

/**
 * init_shm() - Inits a shared memory segment for this process.
 * @shm: Contains all informations for a specific shm. Needs to be initialized
 * 		first with init_shmparam().
 *
 * Creates a shared memory segment based on SHM_KEY value if this one doesn't
 * exist (otherwise get a reference to it) and attaches it to this process.
 * Doesn't initialized the bytes of the shm.
 *
 * Return: 0 if shared memory segment was correctly initialized and attached to
 * 		the process. -1 if one of the syscalls fail.
*/
int init_shm(struct shmparam *shm)
{
	void *tmp;
	struct shmid_ds buf;

	if ((shm->id = shmget(shm->key, shm->size, IPC_CREAT | 0600)) == -1) {
		log_syserr("(shmget)");
		return -1;
	}
	if (shmctl(shm->id, IPC_STAT, &buf) == -1) {
		log_syserr("(shmctl)");
		return -1;
	}
	if ((tmp = shmat(shm->id, NULL, 0)) == (void*)-1) {
		log_syserr("(shmat)");
		return -1;
	}
	shm->addr = (char *)tmp;

	log_verb("Shared memory segment initialized");
	return 0;
}

/**
 * clean_shm() - Detaches or removes a shared memory segment.
 * @shm: Contains all informations for a specific shm. Needs to be initialized
 * 		first with init_shmparam().
 *
 * If this process is the last one attached to the shm, then removes the shm.
 * Otherwise just detaches this process from the shm, but doesn't removes it.
 *
 * Return: 0 on success, -1 on failure.
*/
int clean_shm(struct shmparam *shm)
{
	int nattch;

	if ((nattch = get_shm_nattch(shm->id)) == -1)
		return -1;
	if (nattch <= 1) {
		if (shmctl(shm->id, IPC_RMID, NULL) == -1) {
			log_syserr("(shmctl:IPC_RMID)");
			return -1;
		}
		log_verb("Shared memory segment destroyed");
	} else {
		if (shmdt(shm->addr) == -1) {
			log_syserr("(shmdt)");
			return -1;
		}
		log_verb("Shared memory segment detached");
	}
	return 0;
}

/**
 * get_shm_nattach() - Gets the number of processes attached to a shm.
 * @shm_id: The id of the shared memory segment.
 *
 * Return: Number of process attached currently attached to the shm.
 * 		-1 on failure.
*/
int get_shm_nattch(int shm_id)
{
	struct shmid_ds buf;

	if (shmctl(shm_id, IPC_STAT, &buf) == -1) {
		log_syserr("(shmctl:IPC_STAT)");
		return -1;
	}
	return buf.shm_nattch;
}

