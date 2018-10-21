OBJ = lsh.o Point.o

all: lsh
	rm $(OBJ)

lsh: lsh.o Point.o
	g++ lsh.o Point.o -o lsh

lsh.o: lsh.cpp
	g++ -c lsh.cpp

Point.o: Point.cpp Point.hpp
	g++ -c Point.cpp
