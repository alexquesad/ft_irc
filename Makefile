SRCS =	main.cpp\
		Client.cpp\
		Command.cpp\
		Server.cpp\
		send_replies.cpp

OBJS =	${SRCS:.cpp=.o}

INCLUDES =	Client.hpp\
			Command.hpp\
			Server.hpp\
			Replies.hpp

CC =	c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -I includes

NAME =	server

RM =	rm -rf

%.o:	%.cpp ${INCLUDES}
		${CC} ${CFLAGS} -c $< -o $@

all:	${NAME} Makefile

${NAME}:	${OBJS}	Makefile
			${CC} ${CFLAGS} ${OBJS} -o ${NAME}

clean:
	${RM} ${OBJS}

fclean:		clean
	${RM} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re