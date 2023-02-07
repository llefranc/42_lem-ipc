/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sem_shm.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 15:00:44 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/07 12:44:09 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEM_SHM_H
#define SEM_SHM_H

/**
 * struct shmparam - Contains all information to manage a shared memory segment
 * 		associated with a semaphore set composed of an unique semaphore.
 * @key: Key used to reference a shm accross multiple processus.
 * @id: Id used to interact with a shm for shmat()/shmdt()/shmctl() functions.
 * @size: Size of the shm.
 * @addr: Address of the shm attached to the process.
 * @buf: Buffer to perform temporary operations before read/write the shm.
 * @sem_key: Key used to reference a semaphore set accorss multiple process.
 * @sem_id: Id used to interact with a sem for semop()/semctl() functions.
*/
struct shmparam {
	int key;
	int id;
	int size;
	char *addr;
	char *buf;
	int sem_key;
	int sem_id;
};

int init_shmparam(struct shmparam *shm, int shm_key, int shm_size, int sem_key);

/**
 * clean_shmparam() - Frees the buffer of struct shmparam.
 * @shm: Contains all informations for a specific shm.
*/
static inline void clean_shmparam(struct shmparam *smh)
{
	free(smh->buf);
}

int init_shm(struct shmparam *shm);
int clean_shm(struct shmparam *shm);
int get_shm_nattch(int shm_id);

#endif