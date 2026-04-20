NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++20
RM			= rm -rf

OBJ_DIR		= obj/

SRCS		= main.cpp Server.cpp Client.cpp parse.cpp Pass.cpp\


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
		$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
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