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
#include "point.hpp"
#include "hasher.hpp"

using namespace std;

class hypercube{
  private:
    int k;
    int M;
    int p;
    int dim;
    string metric;

    vector<point> *points;
    vector<vector<point*>*> vertices;

    hasher *hashFunc;
    vector<unordered_map<int, int>*> f;
    int getVertex(point p);

  public:
    hypercube(int k, int M, int p, int dim, string metric, vector<point> *points);
    ~hypercube();

    point *nn(point q, double &minDist);
    unordered_set<point*> rnn(point q, double r);
};

hypercube::hypercube(int k, int M, int p, int dim, string metric, vector<point> *points){
  this->k = k;
  this->M = M;
  this->p = p;
  this->dim = dim;
  this->metric = metric;
  this->points = points;

  //Construct vertices
  int vertexCount = pow(2,k);
  for(int i = 0; i < vertexCount; i++){
    vertices.push_back(new vector<point*>);
  }

  //Construct hash functions
  if(metric.compare("euclidean") == 0){
    hashFunc = new eucl_hash(vertexCount, k, dim);
  }
  else if(metric.compare("cosine") == 0){
    hashFunc = new cos_hash(vertexCount, k, dim);
  }
  else{
    cerr << "Error: Metric \"" << metric << "\" is not supported\n";
  }

  //Initialise f functions
  for(int i = 0; i < k; i++){
    f.push_back(new unordered_map<int,int>());
    f[i]->emplace(0, 0);
    f[i]->emplace(1, 1);
  }

  //Save points to vertices
  for(int i = 0; i < points->size(); i++){
    int key = hashFunc->hash(points->at(i));
    (vertices[key])->push_back(&(points->at(i)));
  }

  for(int i = 0; i < vertexCount; i++){
    cout << "Vertex #" << i << " has " << vertices[i]->size() << " points\n";
  }

}

hypercube::~hypercube(){
  int vertexCount = pow(2,k);
  for(int i = 0; i < vertexCount; i++){
    delete vertices[i];
  }

  for(int i = 0; i < k; i++){
    delete f[i];
  }

  delete hashFunc;
}

int hypercube::getVertex(point p){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  //Initialise random real number generator
  uniform_real_distribution<double> unif(0, 1);
  default_random_engine re(seed);

  vector<int> h = hashFunc->hash_h(p);
  int key = 0;

  for(int i = 0; i < k; i++){
    if(f[i]->find(h[i]) == f[i]->end()){
      //If value h[i] hasn't been hashed yet assign a random value of 0 or 1
      f[i]->emplace(h[i], unif(re) > 0.5 ? 1 : 0);
    }

    key += f[i]->at(h[i]) << i;
  }
  return key;
}

point *hypercube::nn(point q, double &minDist){
  minDist = -1;
  point *nn;

  //Get vertex
  int key = getVertex(q);
  // cout << "new vertex: " << key << "\n";
  bitset<32> binKey(key);
  //Get neighbor vertices
  stack<int> nVertices;
  for(int i = 0; i < k; i++){
    binKey[i] = ~binKey[i];
    nVertices.push(binKey.to_ulong());
    binKey[i] = ~binKey[i];
  }


  int pointsChecked = 0;
  int verticesChecked = 0;

  int i = 0;
  while(pointsChecked < M && verticesChecked < p && !nVertices.empty()){
    if(i < vertices[key]->size()){
      double dist = q.distance(*(vertices[key]->at(i)));
      if(minDist == -1){
        minDist = dist;
        nn = vertices[key]->at(i);
      }
      else if(dist < minDist){
        minDist = dist;
        nn = vertices[key]->at(i);
      }

      i++;
      pointsChecked++;
    }
    else{
      //Go to next vertex
      nVertices.pop();
      if(!nVertices.empty()){
        key = nVertices.top();
      }
      i = 0;

      verticesChecked++;
    }
  }

  return nn;
}

unordered_set<point*> hypercube::rnn(point q, double r){
  unordered_set<point*> rnns;

  //Get vertex
  int key = getVertex(q);
  // cout << "new vertex: " << key << "\n";
  bitset<32> binKey(key);
  //Get neighbor vertices
  stack<int> nVertices;
  for(int i = 0; i < k; i++){
    binKey[i] = ~binKey[i];
    nVertices.push(binKey.to_ulong());
    binKey[i] = ~binKey[i];
  }


  int pointsChecked = 0;
  int verticesChecked = 0;

  int i = 0;
  while(pointsChecked < M && verticesChecked < p && !nVertices.empty()){
    if(i < vertices[key]->size()){
      double dist = q.distance(*(vertices[key]->at(i)));
      if(dist < r){
        rnns.insert(vertices[key]->at(i));
      }

      i++;
      pointsChecked++;
    }
    else{
      //Go to next vertex
      nVertices.pop();
      if(!nVertices.empty()){
        key = nVertices.top();
      }
      i = 0;

      verticesChecked++;
    }
  }

  return rnns;
}


int main(int argc, char* const *argv) {
  //Command line arguments
  char *inputFileName = NULL;
  char *queryFileName = NULL;
  char *outputFileName = NULL;
  int k = 3;
  int M = 10;
  int p = 2;

  char c;

  //Read command line arguments
  while((c = getopt(argc, argv, "d:q:k:M:p:o:")) != -1){
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
      case 'M':
        M = atoi(optarg);
        break;
      case 'p':
        p = atoi(optarg);
        break;
      case 'o':
        outputFileName = optarg;
        break;
      default:
        cerr << "./cube –d <input file> –q <query file> –k <int> -M <int> -p <int> -ο <output file>\n";
        return 1;
    }
  }

  //Check command line arguments
  if(inputFileName == NULL || queryFileName == NULL || outputFileName == NULL){
    cerr << "./cube –d <input file> –q <query file> –k <int> -M <int> -p <int> -ο <output file>\n";
    return 1;
  }
  if(k <= 0){
    cerr << "Error: k can only take positive values";
    return 1;
  }
  if(M <= 0){
    cerr << "Error: M can only take positive values";
    return 1;
  }
  if(p <= 0){
    cerr << "Error: p can only take positive values";
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

  //Initialise hypercube
  hypercube searcher(k, M, p, dim, metric, &points);
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
    unordered_set<point*> lsh_rnns = searcher.rnn(*q, radius);
    //Print results to output file
    outputFile << "R-near neighbors:\n";
    for(unordered_set<point*>::iterator i = lsh_rnns.begin(); i != lsh_rnns.end(); i++){
      outputFile << (*i)->getName() << "\n";
    }

    //Find Nearest Neighbor from LSH
    double lsh_minDist;
    //Get start time
    start = clock();
    point *lsh_nn = searcher.nn(*q, lsh_minDist);
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
