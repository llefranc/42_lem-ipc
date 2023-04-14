/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:42:47 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/14 20:10:46 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shared_rcs.h"
#include "../include/game_utils.h"

#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

/**
 * keygen() - Generates a System V key.
 * @i: To generates different key if needed (need to be incremented each
 *     time a new key is needed).
 *
 * Return: The System V key on success, -1 on error.
*/
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

static inline int clean_shm(int shm_id)
{
	int ret;

	ret = shmctl(shm_id, IPC_RMID, NULL);
	if (ret != -1)
		log_verb("Shared memory segment marked for destroy");
	return ret;
}

static inline int clean_sem(int sem_id)
{
	int ret;

	ret = semctl(sem_id, 0, IPC_RMID);
	if (ret != -1)
		log_verb("Semaphore set removed");
	return ret;
}

static inline int clean_msgq(int msgq_id)
{
	int ret;

	ret = msgctl(msgq_id, IPC_RMID, NULL);
	if (ret != -1)
		log_verb("Message queue removed");
	return ret;
}

/**
 * get_shared_rcs() - Gets or creates all System V ressources.
 * @rcs: Contains all information of System V shared ressources.
 * @key: System V key to access shm/sem/msgq.
 * @shmsize: The size of the shared memory segment.
 *
 * 1. Creates or gets if already existing a shared memory segment and attaches
 *    it to the current process.
 * 2. Creates or gets if already existing a semaphore set of 1 semaphore.
 * 3. Creates or gets if already existing a message queue.
 *
 * Return: 0 on success, -1 on failure. If failure, cleans automatically all
 *         shared ressources.
*/
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
	log_verb("Shared ressources succesfully get");
	return 0;
}

/**
 * Init shared memory segment.
*/
static inline int init_shm(struct shrcs *rcs, struct mapinfo *m)
{
	struct timespec t;

	if (clock_gettime(CLOCK_REALTIME, &t) == -1) {
		log_syserr("(clock_gettime)");
		return -1;
	}
	if (sem_lock(rcs->sem_id) == -1)
		return -1;
	m = (struct mapinfo *)rcs->shm_addr;
	m->start_time = t;
	m->game_state = E_STATE_PRINT;
	if (sem_unlock(rcs->sem_id) == -1)
		return -1;
	return 0;
}

/**
 * init_shared_rcs() - Initializes the System V shared ressources.
* @rcs: Contains all information of System V shared ressources.
 *
 * Initializes the only semaphore of the semaphore set to value 1.
 * Initializes start time value in the shared memory segment with actual time,
 * and game_state to E_STATE_PLAY.
 * The others bytes of shared memory segment are automatically set to 0.
 *
 * Return: 0 on success, -1 on failure.
*/
int init_shared_rcs(struct shrcs *rcs, struct mapinfo **m)
{
	struct semid_ds b = {};
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *__buf;
	} tmp = { .buf = &b };

	*m = (struct mapinfo *)rcs->shm_addr;
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
		if (init_shm(rcs, *m) == -1)
			return -1;
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

/**
 * clean_shared_rcs() - Cleans all the System V shared ressources.
 * @rcs: Contains all information of System V shared ressources.
 * @step: Indicates which System V ressources need to be destroyed. Can be
 *        either one of E_CLEAN_ALL, E_CLEAN_SHM_SEM or E_CLEAN_SHM.
 *
 * Detaches the process from the shared memory segment and destroys the shared
 * ressources if it was the last attached process.
 *
 * Return: 0 on success, a negative number if one or several clean operations
 *         failed.
*/
int clean_shared_rcs(const struct shrcs *rcs, enum clean_step step)
{
	int ret = 0;
	int nbattch;

	if (shmdt(rcs->shm_addr)) {
		log_syserr("(shmdt)");
		ret--;
	}
	if ((nbattch = get_shm_nattch(rcs->shm_id)) == -1)
		return -1;
	if (!nbattch) {
		switch (step) {
		case E_CLEAN_ALL:
			ret += clean_msgq(rcs->msgq_id);
			/* fall through */
		case E_CLEAN_SHM_SEM:
			ret += clean_sem(rcs->sem_id);
			/* fall through */
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