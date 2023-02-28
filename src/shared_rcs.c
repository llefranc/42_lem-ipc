/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:42:47 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/28 15:55:59 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "../include/shared_rcs.h"

static inline int clean_shm(int shm_id);
static inline int clean_sem(int sem_id);
static inline int clean_msgq(int msqg_id);

key_t keygen(int i)
{
	key_t key;
	char buf[1024] = {};

	if (readlink("/proc/self/exe", buf, sizeof(buf)) == -1) {
		log_syserr("(readlink)");
		return -1;
	}
	if ((key = ftok(buf, i)) == -1)
		log_syserr("(ftok)");
	return key;
}

int get_shared_rcs(struct shrcs *rcs, key_t key, size_t shmsize)
{
	if ((rcs->shm_id = shmget(key, shmsize, IPC_CREAT | 0600)) == -1) {
		log_syserr("(shmget)");
		return -1;
	}
	if ((rcs->shm_addr = shmat(rcs->shm_id, NULL, 0)) == (void*)-1) {
		log_syserr("(shmat)");
		clean_shm(rcs->shm_id); /* not using clean_shared_rcs() because nattch = 0 */
		return -1;
	}
	if ((rcs->sem_id = semget(key, 1, IPC_CREAT | 0600)) == -1) {
		log_syserr("(semget)");
		clean_shared_rcs(rcs, E_CLEAN_SHM);
		return -1;
	}
	if ((rcs->msgq_id = msgget(key, IPC_CREAT | 0600)) == -1) {
		log_syserr("(msgget)");
		clean_shared_rcs(rcs, E_CLEAN_SHM_SEM);
		return -1;
	}
	return 0;
}

int init_shared_rcs(const struct shrcs *rcs)
{
	struct semid_ds b = {};
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *__buf;
	} tmp = { .buf = &b };

	if (semctl(rcs->sem_id, 0, IPC_STAT, tmp) == -1) {
		log_syserr("(semctl - IPC_STAT)");
		return -1;
	}
	if (b.sem_otime == 0) { /* To avoid data races for init */
		tmp.val = 1;
		if (semctl(rcs->sem_id, 0, SETVAL, tmp) == -1) {
			log_syserr("(semctl - SETVAL)");
			return -1;
		}
	}
	log_verb("Semaphore initialized to 1");
	return 0;
}

int get_shm_nattch(int shm_id)
{
	struct shmid_ds buf = {};

	if (shmctl(shm_id, IPC_STAT, &buf) == -1) {
		log_syserr("(shmctl - IPC_STAT)");
		return -1;
	}
	return buf.shm_nattch;
}

static inline int clean_shm(int shm_id)
{
	int ret;

	if ((ret = shmctl(shm_id, IPC_RMID, NULL)) == -1)
		log_syserr("(shmctl - IPC_RMID)");
	else
		log_verb("Shared memory segment marked for destroy");
	return ret;
}

static inline int clean_sem(int sem_id)
{
	int ret;

	if ((ret = semctl(sem_id, 0, IPC_RMID)) == -1)
		log_syserr("(semctl - IPC_RMID)");
	else
		log_verb("Semaphore set removed");
	return ret;
}

static inline int clean_msgq(int msgq_id)
{
	int ret;

	if ((ret = msgctl(msgq_id, IPC_RMID, NULL)) == -1)
		log_syserr("(msgctl - IPC_RMID)");
	else
		log_verb("Message queue removed");
	return ret;
}

int clean_shared_rcs(const struct shrcs *rcs, enum clean_step step)
{
	int ret = 0;
	int nbattch;

	if ((nbattch = get_shm_nattch(rcs->shm_id)) == -1)
		return -1;
	if (nbattch == 1) {
		switch (step) {
		case E_CLEAN_ALL:
			ret += clean_msgq(rcs->msgq_id);
			__attribute__ ((fallthrough));
		case E_CLEAN_SHM_SEM:
			ret += clean_sem(rcs->sem_id);
			__attribute__ ((fallthrough));
		case E_CLEAN_SHM:
			ret += clean_shm(rcs->shm_id);
			break;
		default:
			ret = -1;
			break;
		}
	}
	return ret;
}