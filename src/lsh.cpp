 #include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <time.h>
#include <unistd.h> //getopt
#include <stdlib.h> //atoi

#include "point.hpp"
#include "hash_table.hpp"
#include "util.hpp"
#include "searcher.hpp"

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
    return 1;
  }

  if(k <= 0){
    cerr << "Error: k can only take positive values";
    return 1;
  }
  if(L <= 0){
    cerr << "Error: L can only take positive values";
    return 1;
  }

  //Read input file
  vector<point> points;
  int inputDim;
  string metric;
  readInputFile(inputFileName, points, inputDim, metric);

  //Read query file
  vector<point> queries;
  double radius;
  int queryDim;
  readQueryFile(queryFileName, queries, queryDim, radius);

  if(inputDim == -1 || queryDim == -1 || inputDim != queryDim){
    cerr << "Error: Vector dimensions don't match";
  }
  int dim = inputDim;

  //Open output file
  fstream outputFile(outputFileName, ios_base::out);

  //Initialise LSH
  lsh srch(k, L, dim, metric, &points);
  //Timer variables
  clock_t start, end;
  //Statistic variables
  double maxAprox = 0;
  double avrgAprox = 0;
  double avrgLSHt = 0;
  double avrgTruet = 0;

  for(vector<point>::iterator q = queries.begin(); q != queries.end(); q++){
    outputFile << "Query: " << q->getName() << "\n";

    //Find Nearest Neighbors in radius from LSH
    unordered_set<point*> lsh_rnns = srch.rnn(*q, radius);
    //Print results to output file
    outputFile << "R-near neighbors:\n";
    for(unordered_set<point*>::iterator i = lsh_rnns.begin(); i != lsh_rnns.end(); i++){
      outputFile << (*i)->getName() << "\n";
    }

    //Find Nearest Neighbor from LSH
    double lsh_minDist;
    //Get start time
    start = clock();
    point *lsh_nn = srch.nn(*q, lsh_minDist);
    //Get end time
    end = clock();
    //Get duration
    double tLSH = (double) (end - start)/CLOCKS_PER_SEC;
    //Print results to output file
    outputFile << "LSH Nearest neighbor: " << lsh_nn->getName() << "\n";
    outputFile << "distanceLSH: " << lsh_minDist << "\n";
    outputFile << "tLSH: " << tLSH << "\n";

    //Find True Nearest Neighbor
    double true_minDist;
    //Get start time
    start = clock();
    point *true_nn = get_true_nn(*q, true_minDist, &points);
    //Get end time
    end = clock();
    //Get duration
    double tTrue = (double) (end - start)/CLOCKS_PER_SEC;
    //Print results to output file
    outputFile << "Nearest neighbor: " << true_nn->getName() << "\n";
    outputFile << "distanceTrue: " << true_minDist << "\n";
    outputFile << "tTrue: " << tTrue << "\n";

    outputFile << "\n";

    //Keep statistics
    if(lsh_minDist/true_minDist > maxAprox){
      maxAprox = lsh_minDist/true_minDist;
    }
    avrgAprox += lsh_minDist/true_minDist;
    avrgLSHt += tLSH;
    avrgTruet += tTrue;
  }

  //Print statistics
  avrgLSHt = avrgLSHt / (double) queries.size();
  avrgTruet = avrgTruet / (double) queries.size();
  avrgAprox = avrgAprox / (double) queries.size();
  outputFile << "Max approximation ratio = " << maxAprox << "\n";
  outputFile << "Average approximation ratio = " << avrgAprox << "\n";
  outputFile << "Average LSH NN time = " << avrgLSHt << "\n";
  outputFile << "Average True NN time = " << avrgTruet << "\n";

  outputFile.close();
  return 0;
}
