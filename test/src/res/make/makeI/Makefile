
INSTALL_LIB_DIR = "./library"
INSTALL_INC_DIR = "./header"
PRG = libprogram.a
SRC = src.c
OBJ = $(SRC:.c=.o)

all: $(PRG)
$(PRG): $(OBJ)
	ar rcs $(PRG) $(OBJ)
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
	mkdir -p $(INSTALL_LIB_DIR)
	mkdir -p $(INSTALL_INC_DIR)
	cp -f $(PRG) $(INSTALL_LIB_DIR)
	cp -f inc.h $(INSTALL_INC_DIR)
