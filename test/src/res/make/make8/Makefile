
PRG = program
SRC = src.c
OBJ = $(SRC:.c=.o)

all: $(PRG)
$(PRG): $(OBJ)
	gcc $(OBJ) -o $(PRG)
.c.o:
	gcc -c $< -o $@
clean:
	rm -f $(OBJ)
fclean:
	gcc
