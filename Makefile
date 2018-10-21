OBJ = lsh.o point.o

all: lsh
	rm $(OBJ)

lsh: lsh.o point.o
	g++ lsh.o point.o -o lsh

lsh.o: lsh.cpp
	g++ -c lsh.cpp

point.o: point.cpp point.hpp
	g++ -c point.cpp
