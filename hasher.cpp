#include "hasher.hpp"

#include <random>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;


/******************************* Euclidean Hash *******************************/


eucl_hash::eucl_hash(int tableSize, int k, int dim){
  this->tableSize = tableSize;
  this->k = k;
  this->dim = dim;

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  //Initialise random real number generator
  uniform_real_distribution<double> unif_r(0, w);
  //Initialise random int generator
  uniform_int_distribution<int> unif_i(numeric_limits<int>::min(), numeric_limits<int>::max());
  default_random_engine re(seed);

  for(int i = 0; i < k; i++){
    //Generate random single precision real number
    t.push_back(trunc(unif_r(re) * 10)/10);
    //Generate random point following normal distribution in (0,1)
    point p(string("v") + to_string(i), dim, 0, 1);
    v.push_back(p);
    //Generate random ints r
    r.push_back(unif_i(re));
  }
}

int eucl_hash::hash(point p){
  vector<int> h;
  //Calculate every h
  for(int i = 0; i < k; i++){
    h.push_back((p.product(v[i]) + t[i]) / w);
  }

  int key = 0;
  for(int i = 0; i < k; i++){
    key = (key + (((r[i] % tableSize) * (h[i] % tableSize)) % tableSize)) % tableSize;
  }

  key = key < 0 ? -key : key;
  return key;
}


/********************************* Cosine Hash ********************************/


cos_hash::cos_hash(int tableSize, int k, int dim){
  this->tableSize = tableSize;
  this->k = k;
  this->dim = dim;

  for(int i = 0; i < k; i++){
    //Generate point following normal distribution with mean = 0, dev = 1
    r.push_back(new point(string("r") + to_string(i), dim, 0, 1));
  }
}

cos_hash::~cos_hash(){
  for(int i = 0; i < k; i++){
    delete r[i];
  }
}

int cos_hash::hash(point p){
  int key = 0;
  for(int i = 0; i < k; i++){
    key += p.product(*r[i]) >= 0 ? 1<<k : 0;
  }
  return key % tableSize;
}
