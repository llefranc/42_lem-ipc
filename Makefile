# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/30 11:53:43 by llefranc          #+#    #+#              #
#    Updated: 2023/03/02 15:29:02 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	lemipc
CC		=	gcc
FLAGS		=	-O2 -Wall -Werror -Wextra
FLAGS_DEBUG	=	-g -fsanitize=address -Wall -Werror -Wextra -DDEBUG

SRCS		=	main.c shared_rcs.c game.c spawn.c
PATH_SRCS	=	src/
OBJS		=	$(SRCS:.c=.o)

all		:	$(NAME)

debug		:	FLAGS = $(FLAGS_DEBUG)
debug		:	re

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