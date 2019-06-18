# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fchevrey <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/03/13 16:05:39 by fchevrey          #+#    #+#              #
#    Updated: 2019/06/18 18:28:47 by fchevrey         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

## Sources ##
NAME = Glad
SRCS_DIR = srcs

SRCS = glad.c

## Objects ##
OBJS = $(SRCS:.c=.o)

OBJS_PRE = $(addprefix $(MAIN_DIR_PATH), $(OBJS))
SRCS_PRE = $(addprefix $(MAIN_DIR_PATH), $(OBJS))

MAIN_DIR_PATH = $(shell pwd)

HEADER_DIR = includes/

## Includes ##
INC = -I ./$(HEADER_DIR)

HEADER = glad.h khrplatform.h

HEADERS = $(addprefix $(HEADER_DIR), $(HEADER))

## FLAGS ##
CC = gcc

CFLAGS = -Wall -Wextra -Werror

## RULES ##

all:  $(OBJS)

%.o: $(SRCS_DIR)/%.c $(HEADERS)
	@echo "Creating	$@"
	gcc -c -o $@ $< $(CFLAGS) $(INC)

rm_obj:
	@rm -rf $(OBJS)
	@echo "$(NAME)'s object removed"

clean: rm_obj

fclean: rm_obj
	@rm -rf $(NAME)
	@echo "$(NAME) removed"

re: fclean all

.PHONY: all clean fclean re rm_obj 
