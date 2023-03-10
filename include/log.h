/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:47:22 by llefranc          #+#    #+#             */
/*   Updated: 2023/03/02 15:53:40 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef DEBUG
#define log_verb(msg) d_log_verb((msg))
#define log_info(msg) d_log_info((msg))
#define log_warn(msg) d_log_warn((msg))
#define log_err(msg) d_log_err((msg))
#define log_syserr(msg) d_log_syserr((msg))

static inline void d_log_verb(const char *msg)
{
	printf("\033[0;34m[ VERB  ]\033[0m %s\n", msg);
}

static inline void d_log_info(const char* msg)
{
	printf("[ INFO  ] %s\n", msg);
}

static inline void d_log_warn(const char *msg)
{
	printf("\033[0;33m[ WARN  ]\033[0m %s\n", msg);
}

static inline void d_log_err(const char *msg)
{
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s\n", msg);
}

static inline void d_log_syserr(const char *msg)
{
	char *p;

	p = strerror(errno);
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s %s\n", p, msg);
}
#else
static inline void log_verb(const char *msg)
{
	(void)msg;
}

static inline void log_info(const char* msg)
{
	printf("[ INFO  ] %s\n", msg);
}

static inline void log_warn(const char *msg)
{
	printf("\033[0;33m[ WARN  ]\033[0m %s\n", msg);
}

static inline void log_err(const char *msg)
{
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s\n", msg);
}

static inline void log_syserr(const char *msg)
{
	char *p;

	p = strerror(errno);
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s %s\n", p, msg);
}
#endif /* DEBUG */

#endif /* LOG_H */