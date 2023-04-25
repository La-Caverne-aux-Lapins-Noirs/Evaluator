
PRG = program
SRC = src.c
OBJ = $(SRC:.c=.o)

all: $(PRG)
$(PRG): $(OBJ)
	gcc $(OBJ) -o $(PRG)
.c.o:
	gcc -c $< -o $@

clean:
	echo "j'ai tout lavé, crois moi sur parole"
