
PRG = program
SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(OBJ) $(PRG)
$(PRG):
	gcc $(OBJ) -o $(PRG)
.c.o:
	gcc -c $< -o $@

