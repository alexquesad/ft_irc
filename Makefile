NAME = ircserv

SRCS =	srcs/main.cpp\
		srcs/Server.cpp\
		srcs/User.cpp\
		srcs/Channel.cpp\
		srcs/send_replies.cpp\
		srcs/send_msgs.cpp\
		srcs/cmds/NICK.cpp\
		srcs/cmds/JOIN.cpp\
		srcs/cmds/PRIVMSG.cpp\
		srcs/cmds/PART.cpp\
		srcs/cmds/PINGPONG.cpp\
		srcs/cmds/TOPIC.cpp\
		srcs/cmds/KICK.cpp\
		srcs/cmds/KILL.cpp\
		srcs/cmds/QUIT.cpp\
		srcs/cmds/OPER.cpp\
		srcs/cmds/MODE.cpp\
		srcs/cmds/RESTART.cpp \
		srcs/cmds/INVITE.cpp

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

CPPFLAGS = -MMD -I ./includes/

OBJS_DIR = ./objs/

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

DEPS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.d))


all: $(NAME)
	@make $(NAME) -q && echo "All OK !"

$(OBJS_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: clean
	@make all

-include $(DEPS)

.PHONY: all clean flean re






