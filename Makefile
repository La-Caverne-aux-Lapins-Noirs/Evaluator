#################################################################################

 ###############################################################################
 ## Jason Brillante "Damdoshi"                                                ##
 ## Hanged Bunny Studio 2014-2020                                             ##
 ##                                                                           ##
 ##                            - TechnoCentre -                               ##
 ##                                                                           ##
 ###############################################################################

#################################################################################
## Information about the project                                               ##
#################################################################################

  NAME		=	evaluator
  TESTLIB	=	libevaluator.so
  LIBBIN	=	libevaluator.a
  RELEASE	=	1

#################################################################################
## Building details                                                            ##
#################################################################################

  COMPILER	?=	gcc
  LINKER	?=	gcc -o
  TESTLINKER	?=	gcc -shared -fprofile-arcs $(LIB) -o
  LIBLINKER	?=	ar rcs

  BIN_DIR	?=	/usr/local/bin/
  LIB_DIR	?=	/usr/local/lib/
  INC_DIR	?=	/usr/local/include/
  ETC_DIR	?=	/opt/technocore/
  LIB_TESTDIR	?=	$(HOME)/.froot/lib/

  LIB		=	-L$(HOME)/.froot/lib/ -llapin -lcrawler -lsfml-graphics-s \
			-lsfml-audio-s -lsfml-window-s -lsfml-system-s		\
			-ldl -lm -lstdc++ -lavcall -lgcov -rdynamic
  CONFIG	?=	$(FLAG) -fPIC -std=c11 -Wno-format-security		\
			-Wall -Wextra

  DEBUG		=	-O0 -Og -g -g3 -ggdb -fprofile-arcs -ftest-coverage	\
			--coverage -fno-omit-frame-pointer -fno-align-functions	\
			-fno-align-loops
  TEST		=	$(DEBUG) -DNDEBUG
  PRODUCTION	=	-O3 -ffast-math -march=native

  ifeq ($(RELEASE), 2)
    MODE_NAME	=	"Build mode: release"
    PROFILE	=	$(DEBUG)
  else ifeq ($(RELEASE), 1)
    MODE_NAME	=	"Build mode: test"
    PROFILE	=	$(TEST)
  else
    MODE_NAME	=	"Build mode: debug"
    PROFILE	=	$(DEBUG)
  endif

  CP		=	cp -r
  RM		=	rm -f
  ECHO		=	/bin/echo -e
  LOGFILE	=	errors~
  ERRLOG	=	2>> $(LOGFILE)
  MKDIR		=	mkdir -p

  HEADER	=	-I./include/

  DEFLT		=	"\033[00m"
  PINK		=	"\033[1;35m"
  GREEN		=	"\033[0;32m"
  TEAL		=	"\033[1;36m"
  RED		=	"\033[0;31m"

#################################################################################
## Source                                                                      ##
#################################################################################

  SRC		=	$(wildcard src/*.c) $(wildcard src/*/*.c)
  OBJ		=	$(SRC:.c=.o)
  CNF		=	$(wildcard *.dab) $(wildcard *.d)
  INC		=	./include/technocore_api.h

  LIBOBJ	=	$(filter-out src/main.o, $(OBJ))

#################################################################################
## Rules                                                                       ##
#################################################################################

all:			build check
build:			rmlog $(NAME) $(LIBBIN)
$(NAME):		$(OBJ)
			@$(LINKER) $(NAME) $(OBJ) $(LIB) $(ERRLOG) &&		\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(NAME) $(DEFLT) ||	\
			 $(ECHO) $(RED)  "[KO]" $(NAME) $(DEFLT)
			@echo $(MODE_NAME)
$(TESTLIB):		$(OBJ)
			@$(TESTLINKER) $(TESTLIB) $(LIBOBJ) $(ERRLOG) &&	\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(TESTLIB) $(DEFLT) || \
			 $(ECHO) $(RED)  "[KO]" $(TESTLIB) $(DEFLT)
			@echo $(MODE_NAME)
$(LIBBIN):		$(OBJ)
			@$(LIBLINKER) $(LIBBIN) $(LIBOBJ) $(ERRLOG) &&	\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $(LIBBIN) $(DEFLT) || \
			 $(ECHO) $(RED)  "[KO]" $(LIBBIN) $(DEFLT)
			@echo $(MODE_NAME)
.c.o:
			@$(COMPILER) -c $< -o $@ $(PROFILE) $(CONFIG)		\
			 $(HEADER) $(ERRLOG) &&					\
			 $(ECHO) $(TEAL) "[OK]" $(GREEN) $< $(DEFLT) ||	\
			 $(ECHO) $(RED)  "[KO]" $< $(DEFLT)

#################################################################################
## Misc                                                                        ##
#################################################################################

check:			$(TESTLIB)
			@(cd test/ && $(MAKE) --no-print-directory)

install:		$(LIBBIN) $(NAME) check
			@$(MKDIR) $(BIN_DIR) $(LIB_DIR) $(ETC_DIR) $(INC_DIR)
			@$(CP) $(NAME) $(BIN_DIR)
			@$(CP) $(CNF) $(ETC_DIR)
			@$(CP) $(INC) $(INC_DIR)
			@$(CP) $(LIBBIN) $(LIB_DIR)
rmlog:
			@$(RM) $(LOGFILE)
clean:
			@find . -name "*.gc*" -delete
			@$(RM) $(OBJ) &&					\
			 $(ECHO) $(GREEN) "Object file deleted" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in clean rule!" $(DEFLT)
			@(cd test/ && $(MAKE) --no-print-directory clean)
fclean:			clean
			@find .     -name "*.exe" -delete			\
				-or -name "*.a" -delete				\
				-or -name "*.o" -delete				\
				-or -name "*~" -delete
			@$(RM) $(LIB_DIR)/$(LIBBIN)
			@$(RM) $(NAME) &&					\
			 $(ECHO) $(GREEN) "Program deleted!" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in fclean rule!" $(DEFLT)
			@$(RM) $(TESTLIB) &&					\
			 $(ECHO) $(GREEN) "Library deleted!" $(DEFLT) ||	\
			 $(ECHO) $(RED) "Error in fclean rule!" $(DEFLT)
			@(cd test/ && $(MAKE) --no-print-directory fclean)
re:			fclean all
erase:
			@$(RM) $(LOGFILE)
