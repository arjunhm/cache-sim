CC=gcc
CFLAGS=-g -Wall
OPFILE=csim
HFILE=cache.h
IPFILE=cache.c

format:
	clang-format -style="{IndentWidth: 2}" -i $(IPFILE) $(HFILE)

cache:
	$(CC) $(CFLAGS) $(HFILE) $(IPFILE) -o $(OPFILE)

memcheck:
	valgrind --leak-check=full ./$(OPFILE)


