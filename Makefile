CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
OBJ = main.o utils.o bicicletas.o alugueres.o ficheiros.o listagens.o

bikerent: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c constantes.h estruturas.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) bikerent

.PHONY: clean
