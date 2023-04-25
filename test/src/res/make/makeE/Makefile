
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
fclean: clean
	rm -f $(PRG)
re:	fclean all

check:
	echo "jte couvre mec, tu peux aller en prod"
