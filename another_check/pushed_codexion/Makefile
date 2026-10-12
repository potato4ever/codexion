NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = src/main.c \
      src/parse.c \
      src/time.c \
      src/heap.c \
      src/simulation.c \
      src/worker.c \
      src/monitor.c \
      src/dongle_utils.c \
      src/dongle_utils2.c \
      src/heap_utils.c \
      src/monitor_utils.c \
      src/print_event.c \
      src/simulation_destroyer.c \
      src/simulation_initializer.c \
      src/utils.c \
      src/worker_utils.c \
      src/coder_utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c src/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
