CC = gcc
CFLAGS = -Wall -Wextra -O2
LFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

SRC = snake.c lista.c main.c
OBJ = $(SRC:.c=.o)

EXEC = jogo

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LFLAGS)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f *.o $(EXEC)
	