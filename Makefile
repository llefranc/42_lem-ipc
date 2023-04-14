# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/30 11:53:43 by llefranc          #+#    #+#              #
#    Updated: 2023/04/13 16:28:54 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	lemipc
CC		=	gcc
FLAGS		=	-O2 -Wall -Werror -Wextra
FLAGS_DEBUG	=	-g -fsanitize=address -Wall -Werror -Wextra -DDEBUG

SRCS		=	main.c shared_rcs.c game_utils.c spawn_player.c move_player.c
OBJS		=	$(SRCS:.c=.o)
PATH_SRCS	=	src/

HEADS		=	game_utils.h log.h move_player.h shared_rcs.h spawn_player.h
PATH_HEADS	=	include/

all		:	$(NAME)

debug		:	FLAGS = $(FLAGS_DEBUG)
debug		:	re

$(addprefix $(PATH_SRCS), $(OBJS)): $(addprefix $(PATH_HEADS), $(HEADS))

$(NAME)		:	$(addprefix $(PATH_SRCS), $(OBJS))
				$(CC) -o $(NAME) $(FLAGS) $(addprefix $(PATH_SRCS), $(OBJS))

clean		:
			rm -rf $(addprefix $(PATH_SRCS), $(OBJS))

fclean		:	clean
				rm -rf $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re debug

#compiling .o with flags
$(PATH_SRCS)%.o	:	$(PATH_SRCS)%.c
			$(CC) $(FLAGS) -o $@ -c $<