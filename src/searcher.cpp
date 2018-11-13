#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <cmath>
#include <bitset>
#include <random>
#include <iostream>
#include <chrono>

#include "searcher.hpp"
#include "point.hpp"
#include "hasher.hpp"
#include "hash_table.hpp"

using namespace std;

/************************************* LSH ************************************/


lsh::lsh(int k, int L, int dim, string metric, vector<point> *points){
  this->k = k;
  this->L = L;
  this->dim = dim;
  this->metric = metric;
  this->points = points;

  //Construct hash tables
  for(int i = 0; i < L; i++){
    tables.push_back(new hash_table(k, dim, points, metric));
  }
}

lsh::~lsh(){
  for(int i = 0; i < L; i++){
    delete tables[i];
  }
}

point *lsh::nn(point q, double &minDist){
  minDist = -1;
  point *nn;

  for(int i = 0; i < L; i++){
    for(int j = 0; j < k; j++){
      std::vector<point*> *bucket = tables[i]->getBucket(q);

      for(int k = 0; k < bucket->size(); k++){
        double dist = q.distance(*(bucket->at(k)));
        if(minDist == -1){
          minDist = dist;
          nn = bucket->at(k);
        }
        else if(dist < minDist){
          minDist = dist;
          nn = bucket->at(k);
        }
      }
    }
  }

  return nn;
}

unordered_set<point*> lsh::rnn(point q, double r){
  unordered_set<point*> rnns;

  for(int i = 0; i < L; i++){
    for(int j = 0; j < k; j++){
      std::vector<point*> *bucket = tables[i]->getBucket(q);

      for(int k = 0; k < bucket->size(); k++){
        double dist = q.distance(*(bucket->at(k)));
        if(dist < r){
          rnns.insert(bucket->at(k));
        }
      }
    }
  }
  return rnns;
}


/********************************** Hypercube *********************************/


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
