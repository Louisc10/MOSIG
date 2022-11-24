PROG = main
all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

main: main.o imageFormationUtils.o -lm

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

