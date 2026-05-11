NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++20
RM			= rm -rf

OBJ_DIR		= obj/
INCLUDES 	= -I incl

SRCS =	src/main.cpp \
		src/Server.cpp \
		src/Client.cpp \
		src/Channel.cpp \
		src/parse.cpp \
		src/Cleanup.cpp \
		src/commands/Pass.cpp \
		src/commands/Nick.cpp \
		src/commands/User.cpp \
		src/commands/Quit.cpp \
		src/commands/Kick.cpp \
		src/commands/Ping.cpp \
		src/commands/Join.cpp \
		src/commands/Topic.cpp \
		src/commands/Part.cpp \
		src/commands/Invite.cpp \
		src/commands/Privmsg.cpp \
		src/commands/Mode.cpp


OBJS		= $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

DEPS		= $(OBJS:.o=.d)

GREEN		= \033[0;32m
YELLOW		= \033[0;33m
RESET		= \033[0m

all: $(NAME)

$(NAME): $(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
		@echo "$(GREEN)Success: $(NAME) built.$(RESET)"

$(OBJ_DIR)%.o: %.cpp
		@mkdir -p $(dir $@)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@
		@echo "$(YELLOW)Compiling: $<$(RESET)"

.SECONDARY: $(OBJS)

-include $(DEPS)

clean:
		$(RM) $(OBJ_DIR)
		@echo "Objects removed."

fclean: clean
		$(RM) $(NAME)
		@echo "Executable removed."

re: fclean all

.PHONY: all clean fclean re
