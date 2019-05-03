CC=gcc
CFLAGS=-I. -pthread -std=c99
DEPS = s-Mart.h
OBJ = s-Mart.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)