CC = g++
FLG = -std=c++11 -I ./include

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

LSH_OBJ = lsh.o point.o hash_table.o hasher.o util.o
CUBE_OBJ = cube.o point.o hash_table.o hasher.o util.o

all: lsh cube

lsh: $(LSH_OBJ)
	$(CC) $(FLG) $(patsubst %.o, $(OBJDIR)/%.o, $(LSH_OBJ)) -o $(BINDIR)/lsh

cube: $(CUBE_OBJ)
	$(CC) $(FLG) $(patsubst %.o, $(OBJDIR)/%.o, $(CUBE_OBJ)) -o $(BINDIR)/cube

#Recipe for making all object files
%.o: $(SRCDIR)/%.cpp $(INCDIR)/*.hpp
	$(CC) $(FLG) -c $< -o $(OBJDIR)/$@

wc:
	wc $(SRCDIR)/* $(INCDIR)/*

clean:
	rm $(OBJDIR)/* $(BINDIR)/*
