# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/02/05 18:41:15 by llefranc          #+#    #+#              #
#    Updated: 2023/01/31 11:39:23 by llefranc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	libft.a
CC			=	gcc
FLAGS		=	-Wall -Wextra -Werror

LB_SRCS		=	ft_isascii.c ft_lstclear.c ft_lstsize.c ft_memset.c ft_strchr.c \
				ft_strmapi.c ft_tolower.c ft_atoi.c ft_isdigit.c ft_lstdelone.c \
				ft_memccpy.c ft_putchar_fd.c ft_strdup.c ft_strncmp.c ft_toupper.c \
				ft_bzero.c ft_isprint.c ft_lstiter.c ft_memchr.c ft_putendl_fd.c \
				ft_strjoin.c ft_strnstr.c ft_calloc.c ft_itoa.c ft_lstlast.c \
				ft_memcmp.c ft_putnbr_fd.c ft_strlcat.c ft_strrchr.c ft_isalnum.c \
				ft_lstadd_back.c ft_lstmap.c ft_memcpy.c ft_putstr_fd.c ft_strlcpy.c \
				ft_strtrim.c ft_isalpha.c ft_lstadd_front.c ft_lstnew.c ft_memmove.c \
				ft_split.c ft_strlen.c ft_substr.c

LB_OBJS		=	$(LB_SRCS:.c=.o)

INC_PATH	=	include/

LB_PATH		=	src/

all		: 	$(NAME)

$(NAME)	:	$(addprefix $(LB_PATH), $(LB_OBJS))
				ar rc $(NAME) $(addprefix $(LB_PATH), $(LB_OBJS))
				ranlib $(NAME)

clean	:	
				rm -rf $(addprefix $(LB_PATH), $(LB_OBJS))

fclean	:	clean
				rm -rf $(NAME)

re		:	fclean all

.PHONY	:	all clean fclean re

#compiling .o with flags
$(LB_PATH)%.o		:	$(LB_PATH)%.c
			$(CC) $(FLAGS) -o $@ -c $<
