#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <unistd.h> //getopt
#include <stdlib.h> //atoi

#include "Point.hpp"

using namespace std;

int main(int argc, char* const *argv) {
  //Command line arguments
  char *inputFileName = NULL;
  char *queryFileName = NULL;
  char *outputFileName = NULL;
  int k = 4;
  int L = 5;

  char c;

  //Read command line arguments
  while((c = getopt(argc, argv, "d:q:k:L:o:")) != -1){
    switch(c){
      case 'd':
        inputFileName = optarg;
        break;
      case 'q':
        queryFileName = optarg;
        break;
      case 'k':
        k = atoi(optarg);
        break;
      case 'L':
        L = atoi(optarg);
        break;
      case 'o':
        outputFileName = optarg;
        break;
      default:
        cerr << "Usage: ./lsh –d <input file> –q <query file> –k <int> -L <int> -ο <output file>\n";
        return 1;
    }
  }

  //Check command line arguments
  if(inputFileName == NULL || queryFileName == NULL || outputFileName == NULL || k == -1 || L == -1){
    cerr << "Usage: ./lsh –d <input file> –q <query file> –k <int> -L <int> -ο <output file>\n";
  }

  if(k <= 0){
    cerr << "Error: k can only take positive values";
    return 1;
  }
  if(L <= 0){
    cerr << "Error: L can only take positive values";
    return 1;
  }

  //Check files given
  fstream inputFile(inputFileName, ios_base::in);
  //TODO: Check query and output files as well

  //Read and store input dataset
  vector<Point> points;
  int dim;
  int i = 0;

  //Read a line from input file
  string line;
  getline(inputFile, line);
  while(!inputFile.eof()){
    //Create a point from the line read
    Point p(line);

    //Check for consistency of dimensions
    if(i == 0){
      dim = p.dim();
    }
    else{
      if(dim != p.dim()){
        cout << "Error: Not all vectors are of the same dimension\n";
        return 1;
      }
    }
    i++;

    //Store new point
    points.push_back(p);

    //Read next line
    getline(inputFile, line);
  }

  inputFile.close();

  //Print points
  for(vector<Point>::iterator it = points.begin(); it != points.end(); it++){
    it->print();
  }

  return 0;
}
