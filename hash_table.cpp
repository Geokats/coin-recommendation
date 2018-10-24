#include "hash_table.hpp"

#include <random>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;

eucl_hash::eucl_hash(int tableSize, int k, int dim){
  this->tableSize = tableSize;
  this->k = k;
  this->dim = dim;

  //Initialise random real number generator
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  uniform_real_distribution<double> unif(0, w);
  default_random_engine re(seed);

  for(int i = 0; i < k; i++){
    //Generate random single precision real number
    t.push_back(trunc(unif(re) * 10)/10);
    //Generate random point following normal distribution in (0,1)
    point p(string("v") + to_string(i), dim, 0, 1);
    v.push_back(p);
  }
}

int eucl_hash::hash(point p){
  vector<int> h;
  //Calculate every h
  for(int i = 0; i < k; i++){
    h.push_back((p.product(v[i]) + t[i]) / w);
    if(h[i] < 0){
      h[i] = - h[i];
    }
  }

  int key = 0;
  for(int i = 0; i < k; i++){
    key = (key + (h[i] % tableSize)) % tableSize;
  }
  return key;
}


/********************************* Hash Table *********************************/


hash_table::hash_table(int k, int dim, vector<point> *points, string metric){
  tableSize = points->size() / 4;
  //Create hash functions
  hashFunc = new eucl_hash(tableSize, k, dim);
  //Create table
  tableSize = points->size() / 8;
  for(int i = 0; i < tableSize; i++){
    vector<point*> bucket;
    buckets.push_back(bucket);
  }
  //Put points in buckets
  for(int i = 0; i < points->size(); i++){
    buckets[hashFunc->hash(points->at(i))].push_back(&(points->at(i)));
  }
}

hash_table::~hash_table(){
  delete hashFunc;
}

std::vector<point*> *hash_table::getBucket(point p){
  int key = hashFunc->hash(p);
  return &(buckets[key]);
}
