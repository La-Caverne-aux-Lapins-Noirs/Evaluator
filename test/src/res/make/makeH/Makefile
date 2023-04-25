
PRG = program
SRC = src.c
OBJ = $(SRC:.c=.o)
INSTALL_BIN_DIR = "./lol"

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
	mkdir ./tests/report/
install:
	mkdir -p $(INSTALL_BIN_DIR)
	cp -f $(PRG) $(INSTALL_BIN_DIR)
