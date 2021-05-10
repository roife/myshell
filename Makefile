CC = gcc
NAME = myshell.o
CFLAGS = -Wall -Wextra -Wpedantic
OBJS = cmd.o parse.o ps.o main.o

%.o: %.c
	$(CC) -c $(CFLAGS) $<

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

.PHONY: clean

all: $(OBJS)

clean:
	rm -rf *.o
