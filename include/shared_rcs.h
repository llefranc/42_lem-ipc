/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_rcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 17:39:35 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/27 18:30:13 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHARED_RCS
#define SHARED_RCS

#include <sys/types.h>

enum clean_step {
	E_CLEAN_SHM,
	E_CLEAN_SHM_SEM,
	E_CLEAN_ALL,
};

struct shrcs {
	int shm_id;
	int sem_id;
	int msg_id;
	size_t shm_size;
	char *shm_addr;
};

key_t keygen(int i);
int get_shared_rcs(struct shrcs *rcs, key_t key, size_t shmsize);

#endif /* SHARED_RCS */