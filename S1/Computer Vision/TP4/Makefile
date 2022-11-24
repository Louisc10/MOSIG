PROG = clustering clustering_updated
all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

clustering : clustering.o Util.o -lm
clustering_updated : clustering_updated.o Util.o -lm

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

