/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_shm.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 14:58:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/07 11:33:41 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "../include/utils.h"
#include "../include/sem_shm.h"

/**
 * init_shm() - Inits a shared memory segment for this process.
 * @key: Key used to identified the shm for shmget() call.
 * @size: Size expected in bytes for the shm.
 * @shm_id: Set to id of the shm initialized after shmget() call.
 * @shm_addr: Set to the address used to access the shm afer shmat() call.
 *
 * Creates a shared memory segment based on SHM_KEY value if this one doesn't
 * exist (otherwise get a reference to it) and attaches it to this process.
 * Doesn't initialized the bytes of the shm.
 *
 * Return: 0 if shared memory segment was correctly initialized and attached to
 * 		the process. -1 if one of the syscalls fail.
*/
int init_shm(const int key, const int size, int *shm_id, char **shm_addr)
{
	void *tmp;
	struct shmid_ds buf;

	if ((*shm_id = shmget(key, size, IPC_CREAT | 0600)) == -1) {
		log_syserr("(shmget)");
		return -1;
	}
	if (shmctl(*shm_id, IPC_STAT, &buf) == -1) {
		log_syserr("(shmctl)");
		return -1;
	}
	if ((tmp = shmat(*shm_id, NULL, 0)) == (void*)-1) {
		log_syserr("(shmat)");
		return -1;
	}
	*shm_addr = (char *)tmp;

	log_verb("Shared memory segment initialized");
	return 0;
}

/**
 * clean_shm() - Detaches or removes a shared memory segment.
 * @shm_id: The id of the shared memory segment.
 * @shm_addr: The address used to access the shm.
 *
 * If this process is the last one attached to the shm, then removes the shm.
 * Otherwise just detaches this process from the shm, but doesn't removes it.
 *
 * Return: 0 on success, -1 on failure.
*/
int clean_shm(int shm_id, char *shm_addr)
{
	int nattch;

	if ((nattch = get_shm_nattch(shm_id)) == -1)
		return -1;
	if (nattch <= 1) {
		if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
			log_syserr("(shmctl:IPC_RMID)");
			return -1;
		}
		log_verb("Shared memory segment destroyed");
	} else {
		if (shmdt(shm_addr) == -1) {
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

