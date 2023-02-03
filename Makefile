# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/30 11:53:43 by llefranc          #+#    #+#              #
#    Updated: 2023/02/03 14:51:29 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	lemipc
CC		=	gcc
FLAGS_PROD	=	-O2 -Wall -Werror -Wextra
FLAGS_DEBUG	=	-g -fsanitize=address -Wall -Werror -Wextra -DDEBUG

SRCS		=	main.c
PATH_SRCS	=	src/
OBJS		=	$(SRCS:.c=.o)

all	:	$(NAME)

$(NAME)	:	$(addprefix $(PATH_SRCS), $(OBJS))
			$(CC) -o $(NAME) $(FLAGS_DEBUG) $(addprefix $(PATH_SRCS), $(OBJS))

clean	:
		rm -rf $(addprefix $(PATH_SRCS), $(OBJS))

fclean	:	clean
			rm -rf $(NAME)

re	:	fclean all

.PHONY	:	all clean fclean re

#compiling .o with flags
$(PATH_SRCS)%.o	:	$(PATH_SRCS)%.c
			$(CC) $(FLAGS_DEBUG) -o $@ -c $<