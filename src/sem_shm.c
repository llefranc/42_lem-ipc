/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_shm.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 14:58:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/03 18:43:06 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "../include/utils.h"
#include "../include/sem_shm.h"

#define SHM_KEY 1
#define SHM_SIZE 1024

int init_shm(int *shm_id, char **shm_addr)
{
	void *tmp;
	struct shmid_ds buf;

	if ((*shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0600)) == -1) {
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

int get_shm_nattch(int shm_id)
{
	struct shmid_ds buf;

	if (shmctl(shm_id, IPC_STAT, &buf) == -1) {
		log_syserr("(shmctl:IPC_STAT)");
		return -1;
	}
	return buf.shm_nattch;
}

