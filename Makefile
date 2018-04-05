CC = mpicc


SRC = main.c thread_pool.c
HDR = structures.h
OBJ = $(SRC:.c=.o)

all:	hw3

hw3:	$(OBJ)
	@echo LINK $(OBJ) INTO $@
	$(CC) $(OBJ) -o $@

clean:
	rm -f *.o *~ hw3
