CC = g++
FLG = -std=c++11
SRC = lsh.cpp point.cpp point.hpp hash_table.cpp hash_table.hpp hasher.cpp hasher.hpp util.cpp util.hpp
OBJ = lsh.o point.o hash_table.o hasher.o util.o


all: lsh

lsh: lsh.o point.o hash_table.o hasher.o util.o
	$(CC) $(FLG) lsh.o point.o hash_table.o hasher.o util.o -o lsh

lsh.o: lsh.cpp
	$(CC) $(FLG) -c lsh.cpp

point.o: point.cpp point.hpp
	$(CC) $(FLG) -c point.cpp

hash_table.o: hash_table.cpp hash_table.hpp
	$(CC) $(FLG) -c hash_table.cpp

hasher.o: hasher.cpp hasher.hpp
	$(CC) $(FLG) -c hasher.cpp

util.o: util.cpp util.hpp
	$(CC) $(FLG) -c util.cpp

wc:
	wc $(SRC)

clean:
	rm $(OBJ) lsh
