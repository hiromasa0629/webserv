# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/04 01:21:04 by hyap              #+#    #+#              #
#    Updated: 2023/02/04 01:39:39 by hyap             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= main
SRCSDIR		= srcs
SRCS		= $(wildcard $(SRCSDIR)/*.cpp) $(wildcard $(SRCSDIR)/socket/*.cpp)
OBJSDIR		= srcs/obj
OBJS 		= $(SRCS:%.cpp=%.o)
DEPENDSDIR	= srcs/depends
DEPENDS		= $(SRCS:%.cpp=%.d)
CPPFLAGS	= -Wall -Werror -Wextra -Wshadow -std=c++98 -pedantic -Isrcs/server -I srcs/socket
LDFLAGS		= -g -lstdc++ # -fsanitize=address


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $(NAME)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -MMD -c $< -o $@

-include $(DEPENDS)

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPENDS)

fclean: clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all clean fclean re
