# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/04 01:21:04 by hyap              #+#    #+#              #
#    Updated: 2023/03/15 14:01:34 by hyap             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= main
SRCSDIR		= srcs
SRCS		= $(wildcard $(SRCSDIR)/*.cpp) \
				$(wildcard $(SRCSDIR)/config/*.cpp) \
				$(wildcard $(SRCSDIR)/utils/*.cpp) \
				$(wildcard $(SRCSDIR)/socket/*.cpp) \
				$(wildcard $(SRCSDIR)/server/*.cpp) \
				$(wildcard $(SRCSDIR)/logger/*.cpp) \
				$(wildcard $(SRCSDIR)/request/*.cpp) \
				$(wildcard $(SRCSDIR)/response/*.cpp)
OBJSDIR		= srcs/obj
OBJS 		= $(SRCS:%.cpp=%.o)
DEPENDSDIR	= srcs/depends
DEPENDS		= $(SRCS:%.cpp=%.d)
CPPFLAGS	= -Wall -Werror -Wextra -Wshadow -std=c++98 -pedantic
INCLUDES	= -Isrcs/utils \
				-Isrcs/config \
				-Isrcs/utils \
				-Isrcs/server \
				-Isrcs/socket \
				-Isrcs/logger \
				-Isrcs/request \
				-Isrcs/response
LDFLAGS		= -lstdc++ -fsanitize=address -g3

TESTFILE	= tests/test.conf
BANANE		= tests/youpibanane.conf
CC		= g++

all: $(NAME)
# @$(MAKE) clean
	@./main $(TESTFILE)
	
test: $(NAME)
	@./main $(BANANE)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $(NAME)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -MMD -c $< -o $@

-include $(DEPENDS)

clean:
	$(RM) $(OBJS)
	$(RM) $(DEPENDS)

fclean: clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all clean fclean re
