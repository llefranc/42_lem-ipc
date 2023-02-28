/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:42:47 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/27 18:32:39 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shared_rcs.h"

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "../include/utils.h"

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
		// clean
		return -1;
	}
	if ((rcs->sem_id = semget(key, 1, IPC_CREAT | 0600)) == -1) {
		log_syserr("(semget)");
		// clean
		return -1;
	}
	if ((rcs->msg_id = msgget(key, IPC_CREAT | 0600)) == -1) {
		log_syserr("(msgget)");
		// clean
		return -1;
	}
	return 0;
}

