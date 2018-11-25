#include "clusterCreator.hpp"
#include "util.hpp"

#include <iostream>
#include <random>
#include <chrono>
#include <unordered_set>

using namespace std;

clusterCreator::clusterCreator(std::vector<point> *points, configuration conf){
  this->points = points;
  this->k = conf.getClusterCount();

  this->clusters = NULL;

  if(conf.getInitialise() == "random"){
    this->initialise = &clusterCreator::randomInit;
  }
  else{
    cerr << "Initialisation method \"" << conf.getInitialise() << "\" not supported\n";
  }

  if(conf.getAssign() == "lloyds"){
    this->assign = &clusterCreator::lloydsAssign;
  }
  else{
    cerr << "Assignment method \"" << conf.getAssign() << "\" not supported\n";
  }

  if(conf.getUpdate() == "kmeans"){
    this->update = &clusterCreator::kmeansUpdate;
  }
  else{
    cerr << "Update method \"" << conf.getUpdate() << "\" not supported\n";
  }
}

void clusterCreator::makeClusters(){
  (this->*initialise)();
  (this->*assign)();

  for(int i = 0 ; i < 100; i++){
    //Get new centroids
    vector<point> newCentroids = (this->*update)();
    //Check how many centroids have changed
    int unchanged = 0;

    cout << "[";
    for(int i = 0; i < k; i++){
      if(centroids[i].equal(newCentroids[i])){
        cout << "U";
        unchanged++;
      }
      else{
        cout << "C";
      }
    }
    cout << "]\n";

    centroids = newCentroids;
    (this->*assign)();

    if(unchanged == k){
      cout << "At iteration #" << i << " all centroids were unchanged\n";
      break;
    }
  }
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


/*********************************** Update ***********************************/


vector<point> clusterCreator::kmeansUpdate(){
  vector<point> newCentroids;

  for(int i = 0; i < k; i++){
    //Create a new zero-value point
    newCentroids.push_back(point("", centroids[i].dim()));
    for(int j = 0; j < clusters[i].size(); j++){
      newCentroids[i].add(*(clusters[i].at(j)));
    }
    newCentroids[i].div(clusters[i].size());
  }

  return newCentroids;
}


/********************************* Silhouette *********************************/


float clusterCreator::pointSilhouette(point p, int clusterIndex){
  float a = 0;
  float b = -1;

  for(int i = 0; i < k; i++){
    float totalDist = 0;
    for(int j = 0; j < clusters[i].size(); j++){
      totalDist += p.distance(*(clusters[i].at(j)));
    }
    float avgDist = totalDist / (float) clusters[i].size();

    if(k == clusterIndex){
      a = avgDist;
    }
    else if(b == -1 || avgDist < b){
      b = avgDist;
    }
  }

  float s = a < b ? 1.0 - a/b : b/a - 1.0;
  return s;
}

float clusterCreator::clusterSilhouette(int clusterIndex){
  float totalS = 0;
  for(int i = 0; i < clusters[clusterIndex].size(); i++){
    totalS += pointSilhouette(*(clusters[clusterIndex].at(i)), clusterIndex);
  }
  float s = totalS / clusters[clusterIndex].size();
  return s;
}

vector<float> clusterCreator::silhouette(){
  vector<float> scores;
  //Calculate Silhouette score for each cluster
  for(int i = 0; i < k; i++){
    scores.push_back(clusterSilhouette(i));
  }
  return scores;
}
