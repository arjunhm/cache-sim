CC=gcc
CFLAGS=-g -Wall -lm
OPFILE=csim
HFILE=cache.h
IPFILE=cache.c

format:
	clang-format -style="{IndentWidth: 2}" -i $(IPFILE) $(HFILE)

cache:
	$(CC) $(HFILE) $(IPFILE) -o $(OPFILE) $(CFLAGS) 

memcheck:
	valgrind --leak-check=full ./$(OPFILE)


