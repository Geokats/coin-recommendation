CC = g++
FLG = -std=c++11
SRC = lsh.cpp point.cpp point.hpp hash_table.cpp hash_table.hpp
OBJ = lsh.o point.o hash_table.o


all: lsh

lsh: lsh.o point.o hash_table.o
	$(CC) $(FLG) lsh.o point.o hash_table.o -o lsh

lsh.o: lsh.cpp
	$(CC) $(FLG) -c lsh.cpp

point.o: point.cpp point.hpp
	$(CC) $(FLG) -c point.cpp

hash_table.o: hash_table.cpp hash_table.hpp
	$(CC) $(FLG) -c hash_table.cpp

wc:
	wc $(SRC)

clean:
	rm $(OBJ) lsh
