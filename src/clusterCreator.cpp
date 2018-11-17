#include "clusterCreator.hpp"

#include <iostream>
#include <random>
#include <chrono>
#include <unordered_set>

using namespace std;

clusterCreator::clusterCreator(std::vector<point> *points, int k){
  this->points = points;
  this->k = k;

  this->clusters = NULL;

  this->initialise = &clusterCreator::randomInit;
  this->assign = &clusterCreator::lloydsAssign;
}

void clusterCreator::makeClusters(){
  (this->*initialise)();
  (this->*assign)();
}

std::vector<point*>* clusterCreator::getClusters(){
  return clusters;
}

std::vector<point>* clusterCreator::getCentroids(){
  return &centroids;
}


/******************************* Initialisation *******************************/


void clusterCreator::randomInit(){
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  //Initialise random int generator
  uniform_int_distribution<int> unif_i(0, points->size() - 1);
  default_random_engine re(seed);
  //Get k distinct indices of the random centroids
  unordered_set<int> indices;
  while(indices.size() < k){
    indices.emplace(unif_i(re));
  }
  //Save centroids
  for(auto it = indices.begin(); it != indices.end(); it++){
    centroids.emplace_back((*points)[*it]);
  }
}


/********************************* Assignment *********************************/


void clusterCreator::lloydsAssign(){
  //Create new clusters
  if(clusters != NULL){
    delete[] clusters;
  }
  clusters = new std::vector<point*> [k];

  for(auto it = points->begin(); it != points->end(); it++){
    //Find closest centroid
    double minDist = it->distance(centroids[0]);
    int centroid = 0;

    int i = 1;
    while(i < k){
      double dist = it->distance(centroids[i]);
      if(dist < minDist){
        minDist = dist;
        centroid = i;
      }
      i++;
    }

    //Add point to cluster
    clusters[centroid].push_back(&(*it));
  }
}
