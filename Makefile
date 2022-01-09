CFLAGS=-Wall -Wshadow -Wextra -pedantic
LIBS=-lncurses 
CC=gcc

fm: main.c
	$(CC) $(CFLAGS) -o fm main.c functions.c $(LIBS)
