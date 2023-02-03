/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/02 19:47:22 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/03 14:40:54 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef DEBUG
#define log_verb(msg) d_log_verb(__LINE__, __FILE__, (msg))
#define log_info(msg) d_log_info(__LINE__, __FILE__, (msg))
#define log_warn(msg) d_log_warn(__LINE__, __FILE__, (msg))
#define log_err(msg, ret) d_log_err(__LINE__, __FILE__, (msg), (ret))
#define log_syserr(msg, ret) d_log_syserr(__LINE__, __FILE__, (msg), (ret))

static inline int d_log_verb(int line, const char *file, const char *msg)
{
	printf("\033[0;34m[ VERB  ]\033[0m (%s:%d) %s", file, line, msg);
	return 0;
}

static inline int d_log_info(int line, const char *file, const char* msg)
{
	printf("\033[0;37m[ INFO  ]\033[0m (%s:%d) %s", file, line, msg);
	return 0;
}

static inline int d_log_warn(int line, const char *file, const char *msg)
{
	printf("\033[0;33m[ WARN  ]\033[0m (%s:%d) %s", file, line, msg);
	return 0;
}

static inline int d_log_err(int line, const char *file, const char *msg,
		int ret)
{
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m (%s:%d) %s", file,
			line, msg);
	return ret;
}

static inline int d_log_syserr(int line, const char *file, const char *msg,
		int ret)
{
	char *p;

	p = strerror(errno);
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m (%s:%d) %s %s", file,
			line, p, msg);
	return ret;
}
#else
static inline int log_verb(const char *msg)
{
	(void)msg;
	return 0;
}

static inline int log_info(const char* msg)
{
	printf("\033[0;37m[ INFO  ]\033[0m %s", msg);
	return 0;
}

static inline int log_warn(const char *msg)
{
	printf("\033[0;33m[ WARN  ]\033[0m %s", msg);
	return 0;
}

static inline int log_err(const char *msg, int ret)
{
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s", msg);
	return ret;
}

static inline int log_syserr(const char *msg, int ret)
{
	char *p;

	p = strerror(errno);
	fprintf(stderr, "\033[0;31m[ ERROR ]\033[0m %s %s", p, msg);
	return ret;
}
#endif /* DEBUG */

#endif