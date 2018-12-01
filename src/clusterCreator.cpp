#include "clusterCreator.hpp"
#include "point.hpp"
#include "searcher.hpp"
#include "util.hpp"

#include <iostream>
#include <random>
#include <chrono>
#include <unordered_set>

using namespace std;


/******************************* Cost Functions *******************************/


float clusterCost(std::vector<point*> cluster, point centroid, string metric){
  float c = 0;

  for(int i = 0; i < cluster.size(); i++){
    c += distance(centroid, *(cluster[i]), metric);
  }

  return c;
}

/******************************* Cluster Creator ******************************/


clusterCreator::clusterCreator(std::vector<point> *points, configuration conf){
  this->conf = conf;
  this->points = points;
  this->k = conf.getClusterCount();

  this->clusters = NULL;
  this->srch = NULL;

  if(conf.getInitialise() == "random"){
    this->initialise = &clusterCreator::randomInit;
  }
  else if(conf.getInitialise() == "kmeans"){
    this->initialise = &clusterCreator::kmeansInit;
  }
  else{
    cerr << "Initialisation method \"" << conf.getInitialise() << "\" not supported\n";
  }

  if(conf.getAssign() == "lloyds"){
    this->assign = &clusterCreator::lloydsAssign;
  }
  else if(conf.getAssign() == "lsh"){
    //Get dimension of points
    int dim = points->at(0).dim();
    srch = new lsh(conf.getHashFuncCount(), conf.getHashTableCount(), dim, conf.getMetric(), points);
    this->assign = &clusterCreator::rangeSearchAssign;
  }
  else if(conf.getAssign() == "hypercube"){
    //Get dimension of points
    int dim = points->at(0).dim();
    srch = new hypercube(conf.getHCDimension(), conf.getHCMaxPointChecks(), conf.getHCMaxVertexChecks(), dim, conf.getMetric(), points);
    this->assign = &clusterCreator::rangeSearchAssign;
  }
  else{
    cerr << "Assignment method \"" << conf.getAssign() << "\" not supported\n";
  }

  if(conf.getUpdate() == "kmeans"){
    this->update = &clusterCreator::kmeansUpdate;
  }
  else if(conf.getUpdate() == "pam"){
    this->update = &clusterCreator::pamUpdate;
  }
  else{
    cerr << "Update method \"" << conf.getUpdate() << "\" not supported\n";
  }
}

clusterCreator::~clusterCreator(){
  if(this->clusters != NULL){
    delete[] this->clusters;
  }
  if(this->srch != NULL){
    delete this->srch;
  }
}

void clusterCreator::makeClusters(){
  cout << "Initialising clusters...\n";
  centroids = (this->*initialise)();
  clusters = (this->*assign)(centroids);

  cout << "Starting clustering...\n";
  for(int i = 0 ; i < conf.getMaxIterations(); i++){
    //Get new centroids
    vector<point> newCentroids = (this->*update)();
    //Check how many centroids have changed
    int unchanged = 0;

    cout << i+1 << ")\t[";
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

    //Delete old clusters
    delete[] clusters;
    //Update centroids and clusters
    centroids = newCentroids;
    clusters = (this->*assign)(centroids);

    if(unchanged == k){
      cout << "At iteration #" << i+1 << " all centroids were unchanged\n";
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


vector<point> clusterCreator::randomInit(){
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  //Initialise random int generator
  uniform_int_distribution<int> unif_i(0, points->size() - 1);
  default_random_engine re(seed);
  //Get k distinct indices of the random centroids
  unordered_set<int> indices;
  while(indices.size() < k){
    indices.emplace(unif_i(re));
  }
  //Save centroids
  vector<point> centroids;
  for(auto it = indices.begin(); it != indices.end(); it++){
    centroids.emplace_back((*points)[*it]);
  }

  return centroids;
}

vector<point> clusterCreator::kmeansInit(){
  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  //Initialise random int generator
  uniform_int_distribution<int> unif_i(0, points->size() - 1);
  default_random_engine re(seed);

  vector<point> centroids;
  //Get first centroid randomly
  centroids.emplace_back(points->at(unif_i(re)));

  while(centroids.size() < k){
    vector<float> d;
    float dMax = 0;
    for(int i = 0; i < points->size(); i ++){
      float minDist = distance(points->at(i), centroids[0], conf.getMetric());
      for(int j = 1; j < centroids.size(); j++){
        float dist = distance(points->at(i), centroids[j], conf.getMetric());
        if(dist < minDist){
          minDist = dist;
        }
      }
      d.push_back(minDist);

      if(minDist > dMax){
        dMax = minDist;
      }
    }

    vector<float> p;
    p.push_back(d[0]/dMax);
    for(int i = 1; i < points->size(); i ++){
      p.push_back(p[i-1] + d[i]/dMax);

    }

    uniform_real_distribution<float> unif_f(0, p[p.size() - 1]);
    float x = unif_f(re);
    for(int i = 0; i < points->size(); i++){
      if(x <= p[i] && d[i] > 0){
        centroids.emplace_back(points->at(i));
        break;
      }
    }

  }

  return centroids;
}


/********************************* Assignment *********************************/


vector<point*>* clusterCreator::lloydsAssign(std::vector<point> centroids){
  clusters = new vector<point*> [k];

  for(auto it = points->begin(); it != points->end(); it++){
    //Find closest centroid
    int centroid = 0;
    double minDist = distance(*it, centroids[0], conf.getMetric());

    for(int i = 1; i < k; i++){
      double dist = distance(*it, centroids[i], conf.getMetric());
      if(dist < minDist){
        minDist = dist;
        centroid = i;
      }
    }

    //Add point to cluster
    clusters[centroid].push_back(&(*it));
  }

  return clusters;
}

vector<point*>* clusterCreator::rangeSearchAssign(vector<point> centroids){
  clusters = new vector<point*> [k];

  //Calculate min distance between centroids
  float minDist = distance(centroids[0], centroids[1], conf.getMetric());
  for(int i = 0; i < centroids.size(); i++){
    for(int j = 0; j < centroids.size(); j++){
      if(i < j){ //Avoid unecessary calculations
        float dist = distance(centroids[i], centroids[j], conf.getMetric());
        if(dist != 0 && dist < minDist){
          minDist = dist;
        }
      }
    }
  }

  unordered_map<point*, int> assignments;
  float radius = minDist / 2.0;

  while(assignments.size() < points->size() && radius < minDist * 32){
    unordered_map<point*, vector<int>> newAssignments;

    for(int i = 0; i < centroids.size(); i++){
      unordered_set<point*> candidates = srch->rnn(centroids[i], radius);
      for(auto c = candidates.begin(); c != candidates.end(); c++){
        if(assignments.find(*c) == assignments.end()){
          //If candidate hasn't been assigned yet
          if(newAssignments.find(*c) == newAssignments.end()){
            vector<int> v(1, i);
            newAssignments.emplace(*c, v);
          }
          else{
            newAssignments[*c].push_back(i);
          }
        }
      }
    }

    //Resolve points that were close to multiple centroids
    for(auto a = newAssignments.begin(); a != newAssignments.end(); a++){
      int centroid;
      if(a->second.size() > 1){
        centroid = a->second[0];
        float minDist = distance(*(a->first), centroids[a->second[0]], conf.getMetric());
        for(int j = 0; j < a->second.size(); j++){
          float dist = distance(*(a->first), centroids[a->second[j]], conf.getMetric());
          if(dist < minDist){
            minDist = dist;
            centroid = a->second[j];
          }
        }
      }
      else{
        centroid = a->second[0];
      }

      assignments.emplace(a->first, centroid);
    }

    radius *= 2.0;
  }

  cout << "Assigned " << assignments.size() << " points out of " << points->size() << "\n";

  //Assign remaining points
  for(int i = 0; i < points->size(); i++){
    if(assignments.find(&(points->at(i))) == assignments.end()){
      int centroid = 0;
      float minDist = distance(points->at(i), centroids[0], conf.getMetric());
      for(int j = 0; j < centroids.size(); j++){
        float dist = distance(points->at(i), centroids[j], conf.getMetric());
        if(dist < minDist){
          minDist = dist;
          centroid = j;
        }
      }

      assignments.emplace(&(points->at(i)), centroid);
    }
  }

  cout << "Reassignments: Assigned " << assignments.size() << " points out of " << points->size() << "\n";

  //Put points in clusters
  for(auto a = assignments.begin(); a != assignments.end(); a++){
    clusters[a->second].push_back(a->first);
  }

  return clusters;
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

vector<point> clusterCreator::pamUpdate(){
  vector<point> newCentroids;

  for(int i = 0; i < centroids.size(); i++){
    float c = clusterCost(clusters[i], centroids[i], conf.getMetric());
    point centroid = centroids[i];
    for(int j = 0; j < clusters[i].size(); j++){
      //Calculate new cost
      float newC = clusterCost(clusters[i], *(clusters[i].at(j)), conf.getMetric());
      if(newC < c){
        c = newC;
        centroid = *(clusters[i].at(j));
      }
    }
    newCentroids.push_back(centroid);
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
      totalDist += distance(p, *(clusters[i].at(j)), conf.getMetric());
    }
    float avgDist = totalDist / (float) clusters[i].size();

    if(i == clusterIndex){
      a = avgDist;
    }
    else if(b == -1 || avgDist < b){
      b = avgDist;
    }
  }

  float s;
  if(a > b){
    s = (b-a)/a;
  }
  else{
    s = (b-a)/b;
  }

  return s;
}

float clusterCreator::clusterSilhouette(int clusterIndex){
  float s;
  if(clusters[clusterIndex].size() > 1){
    float totalS = 0;
    for(int i = 0; i < clusters[clusterIndex].size(); i++){
      totalS += pointSilhouette(*(clusters[clusterIndex].at(i)), clusterIndex);
    }
    s = totalS / clusters[clusterIndex].size();
  }
  else{
    s = 0;
  }
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
