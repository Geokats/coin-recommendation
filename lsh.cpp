#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <time.h>
#include <unistd.h> //getopt
#include <stdlib.h> //atoi

#include "point.hpp"

using namespace std;

int getPoints(fstream &fs, vector<point> &points){
  int dim;
  int i = 0;
  string line;

  //Read a line from input file
  getline(fs, line);
  while(!fs.eof()){
    //Create a point from the line read
    point p(line);
    //Check for consistency of dimensions
    if(i == 0){
      dim = p.dim();
    }
    else{
      if(dim != p.dim()){
        return -1;
      }
    }
    i++;

    //Store new point
    points.push_back(p);
    //Read next line
    getline(fs, line);
  }
  return dim;
}

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

  //Read and store dataset
  fstream inputFile(inputFileName, ios_base::in);
  vector<point> points;
  if(getPoints(inputFile, points) == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
    return 1;
  }
  inputFile.close();

  //Read and store query points
  fstream queryFile(queryFileName, ios_base::in);
  vector<point> queries;
  if(getPoints(queryFile, queries) == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
    return 1;
  }
  queryFile.close();

  //Open output file
  fstream outputFile(outputFileName, ios_base::out);

  for(vector<point>::iterator q = queries.begin(); q != queries.end(); q++){
    outputFile << "Query: " << q->getName() << "\n";

    //Find True Nearest Neighbor
    double minDist = q->distance(points[0]);
    point nn = *q;
    //Get start time
    clock_t start = clock();

    for(vector<point>::iterator p = points.begin(); p != points.end(); p++){
      double dist = q->distance(*p);
      if(dist < minDist){
        minDist = dist;
        nn = *p;
      }
    }

    //Get end time
    clock_t end = clock();
    //Get duration
    double tTrue = (double) (end - start)/CLOCKS_PER_SEC;

    outputFile << "Nearest neighbor: " << nn.getName() << "\n";
    outputFile << "distanceTrue: " << minDist << "\n";
    outputFile << "tTrue: " << tTrue << "\n";
    outputFile << "\n";
  }

  outputFile.close();
  return 0;
}
