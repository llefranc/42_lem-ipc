# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/30 11:53:43 by llefranc          #+#    #+#              #
#    Updated: 2023/01/30 12:20:52 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	lemipc
CC				=	gcc
FLAGS_PROD      =	-O1 -Wall -Werror -Wextra
FLAGS_DEBUG     =	-g -fsanitize=address -Wall -Werror -Wextra

SRCS			=	main.c
PATH_SRCS		=	src/

OBJS			=	$(SRCS:.c=.o)

all				:	$(NAME)

$(NAME)			:	$(addprefix $(PATH_SRCS), $(SRCS))
						$(CC) -o $(NAME) $(FLAGS) $(addprefix $(PATH_SRCS), $(SRCS))