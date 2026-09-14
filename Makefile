NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = main.c parse.c time.c heap.c simulation.c worker.c monitor.c dongle_utils.c dongle_utils2.c heap_utils.c monitor_utils.c print_event.c simulation_destroyer.c simulation_initializer.c utils.c worker_utils.c coder_utils.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

