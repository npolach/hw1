# cs335 hw1
# to compile your project, type make and press enter
##LIB	= ./libggfonts.so

all: hw1

hw1: hw1.cpp
	g++ hw1.cpp libggfonts.a -Wall -o hw1 -lX11 -lGL -lGLU -lm

clean:
	rm -f hw1
	rm -f *.o

