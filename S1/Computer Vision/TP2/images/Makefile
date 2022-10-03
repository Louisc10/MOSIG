PROG = filtering_3x3, filtering_5x5 filter_median histogram

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

filtering_3x3 : filtering_3x3.o Util.o
filtering_5x5 : filtering_5x5.o Util.o
filter_median : filter_median.o Util.o
histogram : histogram.o Util.o

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

