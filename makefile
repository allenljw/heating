# Makefile for 159.735 Assignment 3
#
# To build just type:
#
# make heat_demo

CPP = g++

# Flags for Mac OSX
#CFLAGS = -w -O3 -arch ppc -arch i386

# FLAGS for Linux
CFLAGS = -O3

# Locally compiled modules
OBJS = fitsfile.o

# Link to CFITSIO libraries - modify these paths accordingly
LIBP = -L/usr/local/lib
INCP = -I/usr/local/include

LIBS = -lcfitsio -lm

MODS = $(INCP) $(LIBP) $(LIBS) $(OBJS) 

BINS = heat

all : $(BINS)

clean :
	rm -f $(BINS)
	rm -f *.o

# Demo program. Add more programs by making entries similar to this
heat : heat_sequential.cpp draw.hxx array.hxx $(OBJS)
	${CPP} $(CFLAGS) -o heat -fopenmp heat_sequential.cpp $(MODS)

# Modules compiled and linked separately
fitsfile.o : fitsfile.cpp fitsfile.h
	${CPP} $(CFLAGS) $(INCP) -c fitsfile.cpp

