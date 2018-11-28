CXX = g++
FLG = -std=c++11 -I ./include -O3

YL_CLR = "\033[1;33m"
NO_CLR = "\033[0m"

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

LSH_OBJ = lsh.o searcher.o point.o hash_table.o hasher.o util.o
CUBE_OBJ = cube.o searcher.o point.o hash_table.o hasher.o util.o
CLUSTER_OBJ = cluster.o clusterCreator.o point.o searcher.o hash_table.o hasher.o util.o

all: lsh cube cluster

lsh: $(BINDIR)/lsh
$(BINDIR)/lsh: $(foreach obj, $(LSH_OBJ), $(OBJDIR)/$(obj))
	@echo "Creating" $(YL_CLR)$@$(NO_CLR) "executable"
	$(CXX) $(FLG) $(patsubst %.o, $(OBJDIR)/%.o, $(LSH_OBJ)) -o $(BINDIR)/lsh

cube: $(BINDIR)/cube
$(BINDIR)/cube: $(foreach obj, $(CUBE_OBJ), $(OBJDIR)/$(obj))
	@echo "Creating" $(YL_CLR)$@$(NO_CLR) "executable"
	$(CXX) $(FLG) $(patsubst %.o, $(OBJDIR)/%.o, $(CUBE_OBJ)) -o $(BINDIR)/cube

cluster: $(BINDIR)/cluster
$(BINDIR)/cluster: $(foreach obj, $(CLUSTER_OBJ), $(OBJDIR)/$(obj))
	@echo "Creating" $(YL_CLR)$@$(NO_CLR) "executable"
	$(CXX) $(FLG) $(patsubst %.o, $(OBJDIR)/%.o, $(CLUSTER_OBJ)) -o $(BINDIR)/cluster

#Recipe for making all object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/*.hpp
	@echo "Creating" $< " ---> " $(YL_CLR)$@$(NO_CLR)
	$(CXX) $(FLG) -c $< -o $@

wc:
	wc $(SRCDIR)/* $(INCDIR)/*

clean:
	rm $(OBJDIR)/* $(BINDIR)/*
