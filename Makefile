# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/30 11:53:43 by llefranc          #+#    #+#              #
#    Updated: 2023/01/31 12:13:09 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	lemipc
CC				=	gcc
FLAGS_PROD      =	-O1 -Wall -Werror -Wextra
FLAGS_DEBUG     =	-g -fsanitize=address -Wall -Werror -Wextra

SRCS			=	main.c
PATH_SRCS		=	src/
OBJS			=	$(SRCS:.c=.o)

PATH_LIB		=	libft/
NAME_LIB		=	libft.a

all		:	$(NAME)

$(NAME)	:	$(addprefix $(PATH_SRCS), $(OBJS))
				$(MAKE) -C $(PATH_LIB)
				$(CC) -o $(NAME) $(FLAGS_DEBUG) $(addprefix $(PATH_SRCS), $(OBJS)) \
				$(addprefix $(PATH_LIB), $(NAME_LIB))
				
clean	:	
				$(MAKE) -C $(PATH_LIB) clean
				rm -rf $(addprefix $(PATH_SRCS), $(OBJS))

fclean	:	clean
				rm -rf $(addprefix $(PATH_LIB), $(NAME_LIB))
				rm -rf $(NAME)

re		:	fclean all

.PHONY	:	all clean fclean re

#compiling .o with flags
$(PATH_SRCS)%.o	:	$(PATH_SRCS)%.c
			$(CC) $(FLAGS_DEBUG) -o $@ -c $<