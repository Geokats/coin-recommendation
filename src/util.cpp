#include "util.hpp"
#include "point.hpp"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;


/***************************** Configuration Class ****************************/


configuration::configuration(){
  //Initialize configuration with default values, or -1 if there is no default
  clusterCount = -1;
  hashFuncCount = 4;
  hashTableCount = 5;

  maxIterations = 100;

  initialise = "random";
  assign = "lloyds";
  update = "kmeans";

  metric = "euclidean";
}

bool configuration::clusterConf(){
  if(clusterCount <= 0 || hashFuncCount <= 0 || hashTableCount <= 0){
    return false;
  }
  else{
    return true;
  }
}


/************************** Utility/Helping Functions *************************/


point *get_true_nn(point q, double &minDist, vector<point> *points){
  minDist = q.distance(points->at(0));
  point *nn = &(points->at(0));

  for(vector<point>::iterator p = points->begin(); p != points->end(); p++){
    double dist = q.distance(*p);
    if(dist < minDist){
      minDist = dist;
      nn = &(*p);
    }
  }

  return nn;
}

int getPoints(fstream &fs, vector<point> &points){
  int dim;
  int i = 0;
  string line;

  //Read a line from input file
  getline(fs, line);
  while(!fs.eof()){
    //Create a point from the line read
    point p(line);
    //Check for consistency of dimensions
    if(i == 0){
      dim = p.dim();
    }
    else{
      if(dim != p.dim()){
        return -1;
      }
    }
    i++;

    //Store new point
    points.push_back(p);
    //Read next line
    getline(fs, line);
  }
  return dim;
}

void readInputFile(string inputFileName, vector<point> &points, int &dim, string &metric){
  //Open input file
  fstream inputFile(inputFileName, fstream::in);
  //Get metric
  getline(inputFile, metric);
  metric.erase(0, 8); //erase "@metric"
  if(metric.find_first_of(" \t\n\r") != string::npos){
    metric.erase(metric.find_first_of(" \t\n\r"), metric.npos);
  }
  cout << "metric = \"" << metric << "\"\n";
  //Read and store dataset
  dim = getPoints(inputFile, points);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  inputFile.close();
}

void readInputFile(string inputFileName, vector<point> &points, int &dim){
  //Open input file
  fstream inputFile(inputFileName, fstream::in);
  //Read and store dataset
  dim = getPoints(inputFile, points);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  inputFile.close();
}

void readQueryFile(string queryFileName, vector<point> &queries, int &dim, double &radius){
  //Open query file
  fstream queryFile(queryFileName, ios_base::in);
  //Get radius
  string line;
  getline(queryFile, line);
  line.erase(0,8);
  radius = atof(line.c_str());
  cout << "radius = " << radius << "\n";
  //Read and store query points
  dim = getPoints(queryFile, queries);
  if(dim == -1){
    cerr << "Error: Not all vectors are of the same dimension\n";
  }
  queryFile.close();
}

void readConfigFile(std::string configFileName, configuration &conf){
  //Open configuration file
  fstream configFile(configFileName, ios_base::in);
  //Read a line from configuration file
  string line;
  getline(configFile, line);
  while(!configFile.eof()){
    //Extract name and value
    istringstream iss(line);
    string var, value;
    getline(iss, var, ' ');
    getline(iss, value, ' ');

    if(var == "number_of_clusters:"){
      conf.setClusterCount(stoi(value));
    }
    else if(var == "number_of_hash_functions:"){
      conf.setHashFuncCount(stoi(value));
    }
    else if(var == "number_of_hash_tables:"){
      conf.setHashTableCount(stoi(value));
    }
    else if(var == "max_iterations:"){
      conf.setMaxIterations(stoi(value));
    }
    else if(var == "initialise:"){
      conf.setInitialise(value);
    }
    else if(var == "assign:"){
      conf.setAssign(value);
    }
    else if(var == "update:"){
      conf.setUpdate(value);
    }
    else if(var == "metric:"){
      conf.setMetric(value);
    }
    else{
      cerr << "Error: Unknown variable \"" << var << "\" given in configuration file\n";
    }

    //Read next line
    getline(configFile, line);
  }

  configFile.close();
}
