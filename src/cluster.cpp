#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <string>
#include <cmath>
#include <bitset>
#include <random>
#include <chrono>
#include <time.h>
#include <unistd.h> //getopt
#include <stdlib.h> //atoi

#include "util.hpp"

using namespace std;

string usageStr = "./cluster -i <input file> -c <configuration file> -o <output file> -d <metric>";

class clusterCreator{
  private:
    int k;
  public:
    
};

int main(int argc, char* const *argv) {
  //Command line arguments
  char *inputFileName = NULL;
  char *configFileName = NULL;
  char *outputFileName = NULL;
  string metric;

  char c;

  //Read command line arguments
  while((c = getopt(argc, argv, "i:c:o:d:")) != -1){
    switch(c){
      case 'i':
        inputFileName = optarg;
        break;
      case 'c':
        configFileName = optarg;
        break;
      case 'o':
        outputFileName = optarg;
        break;
      case 'd':
        metric = string(optarg);
        break;
      default:
        cerr << "Usage: " << usageStr;
        return 1;
    }
  }

  //Check command line arguments
  if(inputFileName == NULL || configFileName == NULL || outputFileName == NULL || metric.size() == 0){
    cerr << "Usage: " << usageStr;
    return 1;
  }
  if(metric != "euclidean" && metric != "cosine"){
    cerr << "Error: Unknown metric \"" << metric << "\"\n";
    return 1;
  }



  return 0;
}
