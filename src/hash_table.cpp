#include "hash_table.hpp"
#include "hasher.hpp"

#include <random>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;


/********************************* Hash Table *********************************/


hash_table::hash_table(int k, int dim, vector<point> *points, string metric){
  tableSize = points->size() / 4;

  //Create hash functions
  if(metric.compare("euclidean") == 0){
    hashFunc = new eucl_hash(tableSize, k, dim);
  }
  else if(metric.compare("cosine") == 0){
    hashFunc = new cos_hash(tableSize, k, dim);
  }
  else{
    cerr << "Error: Metric \"" << metric << "\" is not supported\n";
  }
  //Create table
  for(int i = 0; i < tableSize; i++){
    buckets.push_back(new vector<point*>);
  }
  //Put points in buckets
  for(int i = 0; i < points->size(); i++){
    int key = hashFunc->hash(points->at(i));
    (buckets[key])->push_back(&(points->at(i)));
  }
}

hash_table::~hash_table(){
  delete hashFunc;
  for(int i = 0; i < tableSize; i++){
    delete buckets[i];
  }
}

std::vector<point*> *hash_table::getBucket(point p){
  int key = hashFunc->hash(p);
  return buckets[key];
}
